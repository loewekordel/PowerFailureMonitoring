#ifndef _THINGSPEAKWRPR_H_
#define _THINGSPEAKWRPR_H_

#include <ThingSpeak.h>

/**
 * @brief Thingspeak initialization
 *
 */
void ThingSpeakWrpr_init(void);

/**
 * @brief Thingspeak send data
 *
 * @param data Data to send to channel/field
 * @return uint8 Code of 200 if successful.
               Code of -101 if value is out of range or string is too long (> 255 bytes)
 */
uint8 ThingSpeakWrpr_sendData(sint16 data);

#endif
