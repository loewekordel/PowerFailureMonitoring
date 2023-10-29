#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "debug.h"
#include "secrets.h"
#include "ThingSpeakWrpr.h"

#define INTERVAL (60000) // Time to sleep (in milliseconds)
#define WIFI_MAXRECONATTEMPTS (5u)

bool stWifi = false;
uint8_t cntrWifiRecon = 0u;

Ticker wifiReconnectTimer;
WiFiEventHandler wifiConnectedHandler;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

void connectWifi();
void onWifiConnected(const WiFiEventStationModeConnected &event);
void onWifiGotIp(const WiFiEventStationModeGotIP &event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);

/**
 * @brief connect to Wifi callback
 *
 */
void connectWifi()
{
  LOG("[WiFi] Connecting to Wi-Fi ");
  LOGLN(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

/**
 * @brief Wifi connected callback
 *
 * @param event Wifi Station Mode Connected evend
 */
void onWifiConnected(const WiFiEventStationModeConnected &event)
{
  LOGLN("[WiFi] Wifi connected");
}

/**
 * @brief Wifi got IP callback
 *
 * @param event WiFi Station Mode Got IP event
 */
void onWifiGotIp(const WiFiEventStationModeGotIP &event)
{
  LOG("[WiFi] Got IP: ");
  LOGLN(WiFi.localIP());
  stWifi = true;
}

/**
 * @brief Wifi disconnected callback
 *
 * @param event WiFi Station Mode Disconnected event
 */
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  LOG("[WiFi] Wifi disconnected. Reason: ");
  LOGLN(event.reason);

  switch (event.reason)
  {
  case WIFI_DISCONNECT_REASON_ASSOC_LEAVE:
    break;
  default:
    if (cntrWifiRecon < WIFI_MAXRECONATTEMPTS)
    {
      LOGLN("[WiFi] Try to reconnect");
      cntrWifiRecon++;
    }
    else
    {
      LOGLN("[WiFi] Max. number of reconnect attempts reached. Shutdown and try later");
      ESP.deepSleep((INTERVAL * 10e5), WAKE_RF_DEFAULT); // Sleep for INTERVAL seconds
    }
    break;
  };
}

/**
 * @brief Setup function (Initialization)
 *
 */
void setup()
{
  Serial.begin(115200);
  LOGLN();
  LOGLN();

  // setup wifi callbacks
  wifiConnectedHandler = WiFi.onStationModeConnected(onWifiConnected);
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiGotIp);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  ThingSpeakWrpr_init();
  connectWifi();
}

/**
 * @brief Main function
 *
 */
void loop()
{
  static unsigned long currentMillis = 0;
  static unsigned long previousMillis = 0;

  if (stWifi)
  {
    currentMillis = millis();
    if (currentMillis - previousMillis > INTERVAL)
    {
      previousMillis = currentMillis;
      ThingSpeakWrpr_sendData(1);
    }
  }
}