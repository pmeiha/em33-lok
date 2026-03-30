/*
 * em33-1-mesh.h haederfile
 */

#pragma once

// mesh funktions prototypen

void receivedCallback(const uint32_t &from, const String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);

void meshSendBroadcast(String msg);
void meshMain(void *parameter);

void sendMessage(em33Msg_t *msg); // Prototype so PlatformIO doesn't complain
