/*
 * em33-2-puls.cpp
 *
 * liest die daten des rc-empfängers aus und speichert sie in der globalen variable "em33Msg"
 *
 * Created By: Pius Meier
 * Created on: 20.02.2026
 *
 */

#include "em33-1.h"

#define EM33_1_PULS_LOG false

// em33Msg_t em33Msg; // global variable to store the values read from the RC receiver

void pulsSetup()
{
    em33Msg.newData = false;            // initialize the newData flag to false
    em33Msg.Spannung = 0.0;             // initialize voltage to 0.0
    em33Msg.Strom = 0.0;                // initialize current to 0.0
    em33Msg.speed = 0;                  // initialize speed to 0 (effektiv speed)
    em33Msg.speedp = 0;                 // initialize speed pulse duration to 0 (pulswert der Fernsteuerung)
    em33Msg.brakep = 0;                 // initialize brake pulse duration to 0 (pulswert der Fernsteuerung)
    em33Msg.hostname = EM33_1_HOSTNAME; // set the hostname in the message struct
    pinMode(EM33_PULSE_SPEED_PIN, INPUT);
    pinMode(EM33_PULSE_BRAKE_PIN, INPUT);

    LOG_I(EM33_1_PULS_LOG, "Puls task setup completed");
}

void pulsMain(void *parameter)
{
    pulsSetup();

    while (true)
    {
        unsigned long pulsSpeedDuration;
        unsigned long pulsBrakeDuration;

        // read the duration of the speed signal pulse
        pulsSpeedDuration = pulseIn(EM33_PULSE_SPEED_PIN, HIGH);
        // read the duration of the brake signal pulse
        pulsBrakeDuration = pulseIn(EM33_PULSE_BRAKE_PIN, HIGH);

        // store the values in the global variable "em33Msg"
        if (em33Msg.newData)
        {
            LOG_I(EM33_1_PULS_LOG, "Previous data has not been processed yet. Skipping new data.");
        }
        else
        {
            LOG_I(EM33_1_PULS_LOG, "collect new data.");
            em33Msg.Spannung = random(100, 145) / 10.0; // Placeholder, replace with actual voltage reading if available
            em33Msg.Strom = random(0, 400) / 10.0;      // Placeholder, replace with actual current reading if available
            em33Msg.speed = random(0, 65);
            em33Msg.speedp = pulsSpeedDuration;
            em33Msg.brakep = pulsBrakeDuration;
            em33Msg.newData = true; // set the newData flag to true to indicate that new data has been read
            sendMessage(&em33Msg);  // send the message to the mesh network
        }
        LOG_I(EM33_1_PULS_LOG, String(millis()).c_str());
        LOG_I(EM33_1_PULS_LOG, String("Speed pulse duration: " + String(pulsSpeedDuration) + " ms").c_str());
        LOG_I(EM33_1_PULS_LOG, String("Brake pulse duration: " + String(pulsBrakeDuration) + " ms").c_str());

        tDelay(100); // delay to avoid flooding the log and to give some time for other tasks
    }
}