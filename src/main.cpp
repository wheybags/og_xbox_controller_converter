#include <Arduino.h>
#include "USBHost_t36/USBHost_t36.h"
#include "XboxControllerDriver.hpp"

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
XboxControllerDriver xboxControllerDriver(myusb);

USBDriver *drivers[] = {&hub1, &hub2, &xboxControllerDriver};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1","Hub2","XB"};
bool driver_active[CNT_DEVICES] = {false, false, false};

void mainSetup()
{
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("\n\nUSB Host Testing");
  Serial.println(sizeof(USBHub), DEC);
  myusb.begin();
}

void mainLoop()
{
  myusb.Task();

  for (uint8_t i = 0; i < CNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        Serial.printf("*** Device %s - disconnected ***\n", driver_names[i]);
        driver_active[i] = false;
      } else {
        Serial.printf("*** %d Device %s %x:%x - connected ***\n", i, driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);
      }
    }
  }
}