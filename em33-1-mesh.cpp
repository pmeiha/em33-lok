/*
 * em33-1-mesh.cpp mesh task
 */

#include "em33-1.h"

#define EM33_1_MESH_LOG true

painlessMesh mesh;
// Scheduler userScheduler; // to control your personal task

// Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

int i = 0;

void sendMessage(em33Msg_t *msg)
{
    /*
    String msg = "Hello from node ";
    msg += String(i++);
    msg += " ";
    msg += mesh.getNodeId();
    */
    String jsonString = "";
    StaticJsonDocument<200> doc;
    doc["Spannung"] = msg->Spannung;
    doc["Strom"] = msg->Strom;
    doc["speed"] = msg->speed;
    doc["speedp"] = msg->speedp;
    doc["brakep"] = msg->brakep;
    doc["hostname"] = msg->hostname;
    serializeJson(doc, jsonString);
    LOG_I(EM33_1_MESH_LOG, String(String(millis()) + " Sending message: " + jsonString).c_str());
    // mesh.sendBroadcast(json);
    mesh.sendBroadcast(jsonString);

    msg->newData = false; // reset the newData flag after sending the message
}

void meshSetup()
{
    // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    // mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); // set before init() so that you can see startup messages
    mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

    // Initialize mesh network with auto channel detection
    // Using channel=0 tells the node to scan all channels and auto-detect the mesh
    // This is useful when you have a bridge node that sets the channel dynamically
    // mesh.init(EM33_MESH_PREFIX, EM33_MESH_PASSWORD, &userScheduler, EM33_MESH_PORT, WIFI_AP_STA, 0);
    mesh.init(EM33_MESH_PREFIX, EM33_MESH_PASSWORD, EM33_MESH_PORT, WIFI_AP_STA, 0);

    mesh.stationManual(em33Config.stationSSID, em33Config.stationPassword);

    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    // userScheduler.addTask(taskSendMessage);
    // taskSendMessage.enable();

    mesh.setHostname(EM33_1_HOSTNAME);

    if (!MDNS.begin(EM33_1_HOSTNAME))
    {
        LOG_E(EM33_1_MESH_LOG, "Error setting up MDNS responder!");
    }

    LOG_I(EM33_1_MESH_LOG, "Mesh setup complete");
}

// logt die aktuellen IPs
unsigned long logMeshIP(int interval, unsigned long previousMillis)
{
    IPAddress myIP(0, 0, 0, 0);

    unsigned long now = millis(); // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
    if ((unsigned long)(now - previousMillis) >= interval)
    {                         // check if "interval" ms has passed since last time the clients were updated
        previousMillis = now; // reset previousMillis
        LOG_I(EM33_1_MESH_LOG, String(previousMillis));
        myIP = IPAddress(mesh.getStationIP());
        LOG_II(EM33_1_MESH_LOG, "My IP is ", myIP.toString().c_str());
        myIP = IPAddress(mesh.getAPIP());
        LOG_II(EM33_1_MESH_LOG, "My AP IP is ", myIP.toString().c_str());
    }
    return previousMillis;
}

void meshMain(void *parameter)
{
    int interval = 5000;              // send data to the client every 1000ms -> 1s
    unsigned long previousMillis = 0; // we use the "millis()" command for time reference and this will output an unsigned long

    // it will run the user scheduler as well
    meshSetup();

    for (;;)
    {
        // it will run the user scheduler as well
        mesh.update();
        previousMillis = logMeshIP(interval, previousMillis);
        tDelay(20);
    }
}

// Needed for painlessMesh library
void receivedCallback(const uint32_t &from, const String &msg)
{
    LOG_IH(EM33_1_MESH_LOG, "");
    LOG_IF(EM33_1_MESH_LOG, "startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId)
{
    LOG_I(EM33_1_MESH_LOG, String("--> startHere: New Connection, nodeId = " + String(nodeId)).c_str());
}

void changedConnectionCallback()
{
    LOG_I(EM33_1_MESH_LOG, "Changed connections");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
    LOG_I(EM33_1_MESH_LOG, String("Adjusted time " + String(mesh.getNodeTime()) + ". Offset = " + String(offset)).c_str());
}
