/*
 * em33-2-servoIn.cpp
 *
 * liest die daten des rc-empfängers aus und speichert sie in der globalen variable "em33Msg"
 *
 * Created By: Pius Meier
 * Created on: 20.02.2026
 *
 */

#include "em33-1.h"

#define EM33_1_SERVOIN_LOG false

em33Msg_t em33Msg = {false, 0.0, 0.0, 0, 0, 0, EM33_1_HOSTNAME}; // global variable to store the values read from the RC receiver

ServoInputPin<EM33_PULSE_SPEED_PIN> speed(EM33_SPEED_MIN, EM33_SPEED_MAX);
ServoInputPin<EM33_PULSE_BRAKE_PIN> brake(EM33_BRAKE_MIN, EM33_BRAKE_MAX);

void servoInSetup()
{

    ServoInput.attach(); // attach all inputs

    while (!ServoInput.available())
    { // wait for all signals to be ready
        LOG_I(EM33_1_SERVOIN_LOG, "Waiting for servo signals...");
        tDelay(500);
    }

    LOG_I(EM33_1_SERVOIN_LOG, "ServoIn task setup completed");
}

void servoInMain(void *parameter)
{
    servoInSetup();

    while (true)
    {
        unsigned long pulsSpeedDuration;
        unsigned long pulsBrakeDuration;

        // read the duration of the speed signal pulse
        pulsSpeedDuration = speed.getPulse();
        // read the duration of the brake signal pulse
        pulsBrakeDuration = brake.getPulse();

        // store the values in the global variable "em33Msg"
        if (em33Msg.newData)
        {
            LOG_I(EM33_1_SERVOIN_LOG, "Previous data has not been processed yet. Skipping new data.");
        }
        else
        {
            // LOG_I(EM33_1_SERVOIN_LOG, "collect new data.");
            em33Msg.Spannung = random(100, 145) / 10.0; // Placeholder, replace with actual voltage reading if available
            em33Msg.Strom = random(0, 400) / 10.0;      // Placeholder, replace with actual current reading if available
            em33Msg.speed = random(0, 65);
            em33Msg.speedp = pulsSpeedDuration;
            em33Msg.brakep = pulsBrakeDuration;
            em33Msg.newData = true; // set the newData flag to true to indicate that new data has been read
            sendMessage(&em33Msg);  // send the message to the mesh network
        }
        // LOG_I(EM33_1_SERVOIN_LOG, String(millis()).c_str());
        LOG_I(EM33_1_SERVOIN_LOG, String("Speed pulse duration: " + String(pulsSpeedDuration) + " us " + "Brake pulse duration: " + String(pulsBrakeDuration) + " us").c_str());
        // LOG_I(EM33_1_SERVOIN_LOG, String("Brake pulse duration: " + String(pulsBrakeDuration) + " ms").c_str());

        tDelay(100); // delay to avoid flooding the log and to give some time for other tasks
    }
}