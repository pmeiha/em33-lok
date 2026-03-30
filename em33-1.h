/*
 * em33-2.h haederfile für den ersten Sitzwagen
 */

#pragma once

#include <WiFi.h>
#include <AsyncTCP.h>
#include <painlessMesh.h>
#include <ServoInput.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
// #include <WebSocketsServer.h>

#include "em33.h"
#include "em33-1-mesh.h"
#include "em33-1-servoIn.h"
#include "em33-1-display.h"

extern int em33StatusCode1;
extern int em33StatusCode2;
extern int em33StatusCode3;
extern int em33StatusCode4;
extern int em33StatusCode5;
extern int em33StatusCode6;

extern em33Config_t em33Config;

#define EM33_HOSTNAME EM33_1_HOSTNAME
