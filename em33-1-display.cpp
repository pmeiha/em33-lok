/**
 * @file em33-1-display.cpp
 * @brief display und mess task for em33-1 project
 *
 *
 * @author Pius Meier @version 1.0.0 basefunctions
 *
 */

#include "em33-1.h"

#include "MAX72XX.h"

#define EM33_1_DISPLAY_LOG true
constexpr uint8_t numDevices = 1;      // the number of devices cascaded together
constexpr uint8_t digitsPerDevice = 8; // the number of digits on each device
constexpr uint8_t latchPin = 7;        // change to reflect your latchPin

uint8_t intensity = 6; // set the intensity (brightness) of the display (0 is min, 15 is max)
float V = 0.0;         // voltage
float A = 0.0;         // current
int S = 0;             // speed in km/h

// for hardware SPI
// create the object for the device chain
MAX72XX dispChain(latchPin, numDevices);

// for software SPI
// constexpr uint8_t dataPin = 6;   // change to reflect your dataPin
// constexpr uint8_t clockPin = 4;  // change to reflect your clockPin

// create the object for the device chain
// MAX72XX dispChain(dataPin, clockPin, latchPin, numDevices);

// create the 3 displays V / km/h / A
MAX72XXDisplay VDisp(&dispChain, 5, 3);
MAX72XXDisplay SDisp(&dispChain, 3, 2);
MAX72XXDisplay ADisp(&dispChain, 0, 3);

void displaySetup()
{

    setIntensity(em33Config.ledIntensity); // set the intensity for all the devices

    VDisp.fill(MAX72XX::Character::BLANK);
    VDisp.writeNumber(V, MAX72XX::Character::BLANK, 1);
    // VDisp.update();

    SDisp.fill(MAX72XX::Character::BLANK);
    SDisp.writeChar(0, 0, true);
    SDisp.writeChar(1, (uint8_t)MAX72XX::Character::BLANK, true);
    SDisp.update();

    ADisp.fill(MAX72XX::Character::BLANK);
    ADisp.writeNumber(A, MAX72XX::Character::BLANK, 1);
    // ADisp.update();

    LOG_I(EM33_1_DISPLAY_LOG, "MAX72xx setup completed");
}

void setIntensity(int8_t intensity)
{
    dispChain.setIntensity(intensity);
}

bool displayStatus(int statusCode, unsigned int *i, unsigned int *p, bool *led)
{
    bool retVal = false;

    if (*i < statusCode)
    {
        if (!*led)
            *led = true;
        else
        {
            retVal = true;
            *led = false;
            (*i)++;
        }
    }
    else
    {
        if (*p > 0)
        {
            (*p)--;
        }
        else
        {
            *i = 0;
            *p = 3;
        }
    }
    return retVal;
}

void displayMain(void *parameter)
{
    unsigned int ii = 0;

    unsigned int i1 = 0;
    unsigned int i2 = 0;
    unsigned int i3 = 0;
    unsigned int i4 = 0;
    unsigned int i5 = 0;
    unsigned int i6 = 0;
    unsigned int p1 = 0;
    unsigned int p2 = 0;
    unsigned int p3 = 0;
    unsigned int p4 = 0;
    unsigned int p5 = 0;
    unsigned int p6 = 0;
    bool led1 = false;
    bool led2 = false;
    bool led3 = false;
    bool led4 = false;
    bool led5 = false;
    bool led6 = false;

    displaySetup();

    while (true)
    {

        if (++ii % 40 == 0)
        {
            LOG_II(EM33_1_DISPLAY_LOG, "Updating display...", String(ii).c_str());
            V = random(0, 145) / 10.0; // Placeholder, replace with actual voltage reading if available
            A = random(0, 100) / 10.0; // Placeholder, replace with actual current reading if available
            S = random(0, 20);         // Placeholder, replace with actual speed reading if available
            ii = 0;
        }

        VDisp.writeNumber(V, MAX72XX::Character::BLANK, 1);

        if (S == 0)
        {
            SDisp.writeChar(0, 0, true);
            SDisp.writeChar(1, (uint8_t)MAX72XX::Character::BLANK, true);
            SDisp.update();
        }
        else
        {
            SDisp.writeNumber(S, MAX72XX::Character::BLANK);
        }

        ADisp.writeNumber(A, MAX72XX::Character::BLANK, 1);

        if (em33Config.blinkStatus)
        {
            if (displayStatus(em33StatusCode1, &i1, &p1, &led1))
                ADisp.setDecimalPoint(0, true);
            if (displayStatus(em33StatusCode2, &i2, &p2, &led2))
                ADisp.setDecimalPoint(2, true);
            if (displayStatus(em33StatusCode3, &i3, &p3, &led3))
                SDisp.setDecimalPoint(0, true);
            if (displayStatus(em33StatusCode4, &i4, &p4, &led4))
                SDisp.setDecimalPoint(1, true);
            if (displayStatus(em33StatusCode5, &i5, &p5, &led5))
                VDisp.setDecimalPoint(0, true);
            if (displayStatus(em33StatusCode6, &i6, &p6, &led6))
                VDisp.setDecimalPoint(2, true);
        }

        tDelay(250); // update every 250 ms
    }
}
