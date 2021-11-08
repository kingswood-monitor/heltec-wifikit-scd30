#include "pb.h"
#include "pb_common.h"
#include "pb_encode.h"
#include <Arduino.h>
#include <secrets.h>

#include <kwHeltecWifikit32.h>
#include <kwSCD30.h>
#include <kwSimpletimer.h>
#include <packet.pb.h>

#define SENSOR_TYPE        "energy"
#define FIRMWARE_VERSION   "3.0.1"
#define TEMPERATURE_OFFSET 2.5
#define PROTO_BUF_LENGTH   100

struct HeltecConfig config = {
    .ap1 = accessPoint{ WIFI_SSID1, WIFI_PASSWORD1 },
    .ap2 = accessPoint{ WIFI_SSID2, WIFI_PASSWORD2 },
    .ap3 = accessPoint{ WIFI_SSID3, WIFI_PASSWORD3 },
    .rotateDisplay = true,
    .firmwareVersion = FIRMWARE_VERSION,
};

kwHeltecWifikit32 heltec{ config };
kwSCD30           scd30;
kwSimpletimer     timer;
AsyncWebServer    server( 80 );
AsyncWebSocket    ws( "/ws" );

uint32_t packetID = 0;
uint8_t  tempFieldID;
uint8_t  humidityFieldID;
uint8_t  CO2FieldID;

bool encodeField( pb_ostream_t *ostream, const pb_field_iter_t *field,
                  SensorField *sensorField )
{
  if ( !( pb_encode_tag_for_field( ostream, field ) &&
          pb_encode_submessage( ostream, SensorField_fields, sensorField ) ) )
  {
    log_e( "Failed to encode temperature field: %s ", ostream->errmsg );
    return false;
  }
  return true;
}

bool encodeFields( pb_ostream_t *ostream, const pb_field_iter_t *field,
                   void *const *arg )
{
  bool isOK = false;

  SensorField sensorField = SensorField_init_default;
  sensorField.sensor_name = SensorName_SCD30;

  sensorField.which_type = SensorField_temperature_tag;
  sensorField.type.temperature = scd30.temperature() - TEMPERATURE_OFFSET;
  isOK |= encodeField( ostream, field, &sensorField );

  sensorField.which_type = SensorField_humidity_tag;
  sensorField.type.humidity = scd30.humidity();
  isOK |= encodeField( ostream, field, &sensorField );

  sensorField.which_type = SensorField_co2_tag;
  sensorField.type.humidity = scd30.co2();
  isOK |= encodeField( ostream, field, &sensorField );

  return isOK;
}

void publishEvent()
{
  if ( scd30.dataAvailable() )
  {
    uint8_t      buffer[PROTO_BUF_LENGTH] = { 0 };
    pb_ostream_t ostream = pb_ostream_from_buffer( buffer, sizeof( buffer ) );

    Packet packet = Packet_init_default;
    Meta   sensorMeta = Meta_init_default;
    strcpy( sensorMeta.device_id, heltec.deviceID );
    strcpy( sensorMeta.firmware, FIRMWARE_VERSION );
    packet.has_meta = true;
    packet.measurement_type = MeasurementType_environment;
    packet.meta = sensorMeta;
    packet.packet_id = packetID++;

    packet.sensorFields.funcs.encode = encodeFields;

    if ( !pb_encode( &ostream, Packet_fields, &packet ) )
    {
      log_e( "Failed to encode: %s", ostream.errmsg );
      return;
    }

    // for ( int i = 0; i < ostream.bytes_written; i++ )
    // {
    //   Serial.printf( "%02X", buffer[i] );
    // }

    ws.binaryAll( buffer, ostream.bytes_written );

    heltec.update( tempFieldID, scd30.temperature() );
    heltec.update( humidityFieldID, scd30.humidity() );
    heltec.update( CO2FieldID, scd30.co2() );

    log_i( "Temp: %.1f, Humidity: %d, CO2: %d", scd30.temperature(),
           scd30.humidity(), scd30.co2() );
  }
}

void setup()
{
  Serial.begin( 115200 );

  tempFieldID = heltec.addDisplayField( displayField{ "temp", "degC" } );
  humidityFieldID = heltec.addDisplayField( displayField{ "hum", "%" } );
  CO2FieldID = heltec.addDisplayField( displayField{ "co2", "ppm" } );
  heltec.init();

  scd30.start( TEMPERATURE_OFFSET );

  ws.onEvent( onWsEvent );
  server.addHandler( &ws );
  server.begin();

  timer.setInterval( 1000L, publishEvent );
}

void loop()
{
  heltec.run();
  timer.run();
}