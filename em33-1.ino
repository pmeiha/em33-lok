/*
 * em33-1.ino main file für die em33-lok
 */

#include "em33-1.h"

// enable log messages for this file
#define EM33_1LOG true

TaskHandle_t meshTask = NULL;
TaskHandle_t servoInTask = NULL;
TaskHandle_t pulsTask = NULL;
TaskHandle_t displayTask = NULL;

int em33StatusCode1 = 0;
int em33StatusCode2 = 0;
int em33StatusCode3 = 0;
int em33StatusCode4 = 0;
int em33StatusCode5 = 0;
int em33StatusCode6 = 2;

em33Config_t em33Config;

void saveConfig()
{
  if (FS_present)
  {
    DynamicJsonDocument doc(512);
    doc["version"] = em33Config.version;
    doc["ledIntensity"] = em33Config.ledIntensity;
    doc["hostname"] = em33Config.hostname;
    doc["stationSSID"] = em33Config.stationSSID;
    doc["stationPassword"] = em33Config.stationPassword;
    doc["blinkStatus"] = em33Config.blinkStatus;

    File configFile = LittleFS.open("/config.json", "w");
    if (configFile)
    {
      serializeJson(doc, configFile);
      configFile.close();
      LOG_I(EM33_1LOG, "Config saved successfully");
    }
    else
    {
      LOG_I(EM33_1LOG, "Failed to open config file for writing");
    }
  }
  else
  {
    LOG_I(EM33_1LOG, "FS not present, cannot save config");
  }
}

void saveIntensity(int8_t inten)
{

  em33Config.ledIntensity = inten;
  setIntensity(inten);
  saveConfig();
}

void saveBlinkStatus(bool status)
{

  em33Config.blinkStatus = status;
  saveConfig();
}

void saveSSID(String ssid)
{

  em33Config.stationSSID = ssid;
  saveConfig();
}

void savePWD(String pwd)
{

  em33Config.stationPassword = pwd;
  saveConfig();
}

void createConfig()
{

  em33Config.version = 1;
  em33Config.ledIntensity = 6;
  em33Config.hostname = EM33_1_HOSTNAME;
  em33Config.stationSSID = EM33_STATION_SSID;
  em33Config.stationPassword = EM33_STATION_PASSWORD;
  em33Config.blinkStatus = true;

  saveConfig();
}

void loadConfig()
{
  if (FS_present)
  {
    File configFile = LittleFS.open("/config.json", "r");
    if (configFile)
    {
      size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc, buf.get());
      if (!error)
      {
        em33Config.version = doc["version"].as<int>();
        em33Config.ledIntensity = doc["ledIntensity"].as<int>();
        em33Config.hostname = doc["hostname"].as<String>();
        em33Config.stationSSID = doc["stationSSID"].as<String>();
        em33Config.stationPassword = doc["stationPassword"].as<String>();
        em33Config.blinkStatus = doc["blinkStatus"].as<bool>();
        LOG_I(EM33_1LOG, "Config loaded successfully");
      }
      else
      {
        LOG_I(EM33_1LOG, "Failed to parse config file");
        createConfig();
      }
    }
    else
    {
      LOG_I(EM33_1LOG, "No config file found, using default values");
      createConfig();
    }
  }
  else
  {
    LOG_I(EM33_1LOG, "FS not present, cannot load config");
    createConfig();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(5000);
  LOG_I(EM33_1LOG, "start setup");
  pinMode(LED_BUILTIN, OUTPUT);

  startFilehandler();

  loadConfig();

  // initialize and start mesh network
  xTaskCreatePinnedToCore(
      meshMain,   // funktion
      "meshMain", // task name
      4096,       // stacksize 9k
      NULL,       // parameter
      1,          // priorität
      &meshTask,  // taskhandel
      0);         // Core

  // initialize and start webserver (is allready a task)
  // muss nach dem mesh task gestartet werden, da der webserver die mesh funktionalität nutzt
  webServerSetup();

  // start puls task
  xTaskCreatePinnedToCore(
      servoInMain,    // funktion
      "servioInMain", // task name
      4096,           // stacksize 9k
      NULL,           // parameter
      1,              // priorität
      &servoInTask,   // taskhandel
      0);             // Core

  xTaskCreatePinnedToCore(
      displayMain,   // funktion
      "displayMain", // task name
      4096,          // stacksize 9k
      NULL,          // parameter
      1,             // priorität
      &displayTask,  // taskhandel
      0);            // Core

  LOG_I(EM33_1LOG, "setup completed");
}

void loop()
{
  unsigned int i;

  for (i = 0; i <= em33StatusCode1; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    tDelay(250);
    digitalWrite(LED_BUILTIN, LOW);
    tDelay(250);
  }
  tDelay(1750);
}
