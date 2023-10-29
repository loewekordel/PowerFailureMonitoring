#include <ESP8266WiFi.h>
#include "ThingSpeakWrpr.h"
#include "debug.h"
#include "secrets.h"

#define THINGSPEAKWRPR_FIELDNR (1u)

WiFiClient client;

/**
 * @brief Thingspeak initialization
 *
 */
void ThingSpeakWrpr_init(void)
{
  ThingSpeak.begin(client);
}

/**
 * @brief Thingspeak send data
 *
 * @param data Data to send to channel/field
 * @return uint8 Code of 200 if successful.
                 Code of -101 if value is out of range or string is too long (> 255 bytes)
 */
uint8 ThingSpeakWrpr_sendData(sint16 data)
{
  uint8 rtn = 0;

  // ThingSpeak.begin(client);
  rtn = ThingSpeak.setField(THINGSPEAKWRPR_FIELDNR, data);
  if (rtn != TS_OK_SUCCESS)
  {
    LOG("Error: ThingSpeak error number: ");
    LOGLN(rtn);
    return rtn;
  }
  rtn = ThingSpeak.writeFields(THINGSPEAK_CHNR, THINGSPEAK_APIKEY);
  if (rtn != TS_OK_SUCCESS)
  {
    LOG("Error: ThingSpeak error number: ");
    LOGLN(rtn);
    return rtn;
  }

  LOGLN("[TS] Data sent");
  return 0;
}