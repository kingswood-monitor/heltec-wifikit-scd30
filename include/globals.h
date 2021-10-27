#ifndef H_GLOBALS
#define H_GLOBALS

extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}

#include <kwSCD30.h>
#include <kwLED.h>

#define COL2 50

const char *g_firmwareVersion = "1.1.0";
char g_deviceID[16] = { 0 };
int g_sampleIntervalSeconds = 2;

kwSCD30 scd30;

kwLED led(LED);

#endif