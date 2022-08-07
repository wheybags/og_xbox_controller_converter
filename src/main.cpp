#include <Arduino.h>
#include <USBHost_t36.h>
#include <XInput.h>
#include "XboxControllerDriver.hpp"

USBHost usbHost;
USBHub hub1(usbHost);
USBHub hub2(usbHost);
XboxControllerDriver xboxControllerDriver(usbHost);

USBDriver *drivers[] = {&hub1, &hub2, &xboxControllerDriver};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1","Hub2","XB"};
bool driver_active[CNT_DEVICES] = {false, false, false};

void mainSetup()
{
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("\n\nUSB Host Testing");
  Serial.println(sizeof(USBHub), DEC);
  usbHost.begin();

  XInput.begin();
  XInput.setAutoSend(false);
  XInput.setJoystickRange(-32768, 32767);
  XInput.setTriggerRange(0, 255);
}

void mainLoop()
{
  usbHost.Task();

  for (uint8_t i = 0; i < CNT_DEVICES; i++)
  {
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

//  XInput.press(BUTTON_A);
//	delay(1000);
//
//	XInput.release(BUTTON_A);
//	delay(1000);

    OGXINPUT_GAMEPAD gamepad = xboxControllerDriver.getLatestReport();
    XInput.setButton(XInputControl::BUTTON_A, gamepad.bAnalogButtons[OGXINPUT_GAMEPAD_A] > 0);
    XInput.setButton(XInputControl::BUTTON_B, gamepad.bAnalogButtons[OGXINPUT_GAMEPAD_B] > 0);
    XInput.setButton(XInputControl::BUTTON_X, gamepad.bAnalogButtons[OGXINPUT_GAMEPAD_X] > 0);
    XInput.setButton(XInputControl::BUTTON_Y, gamepad.bAnalogButtons[OGXINPUT_GAMEPAD_Y] > 0);

    XInput.setJoystick(XInputControl::JOY_LEFT, gamepad.sThumbLX, gamepad.sThumbLY);
    XInput.setJoystick(XInputControl::JOY_RIGHT, gamepad.sThumbRX, gamepad.sThumbRY);

    XInput.setButton(XInputControl::BUTTON_L3, gamepad.wButtons & OGXINPUT_GAMEPAD_LEFT_THUMB);
    XInput.setButton(XInputControl::BUTTON_R3, gamepad.wButtons & OGXINPUT_GAMEPAD_RIGHT_THUMB);

    XInput.setTrigger(XInputControl::TRIGGER_LEFT, gamepad.bAnalogButtons[OGXINPUT_GAMEPAD_LEFT_TRIGGER]);
    XInput.setTrigger(XInputControl::TRIGGER_RIGHT, gamepad.bAnalogButtons[OGXINPUT_GAMEPAD_RIGHT_TRIGGER]);

    XInput.setButton(XInputControl::BUTTON_LB, gamepad.bAnalogButtons[OGXINPUT_GAMEPAD_WHITE] > 0);
    XInput.setButton(XInputControl::BUTTON_RB, gamepad.bAnalogButtons[OGXINPUT_GAMEPAD_BLACK] > 0);

    XInput.setDpad(gamepad.wButtons & OGXINPUT_GAMEPAD_DPAD_UP,
                   gamepad.wButtons & OGXINPUT_GAMEPAD_DPAD_DOWN,
                   gamepad.wButtons & OGXINPUT_GAMEPAD_DPAD_LEFT,
                   gamepad.wButtons & OGXINPUT_GAMEPAD_DPAD_RIGHT,
                   false);

    XInput.setButton(XInputControl::BUTTON_BACK, gamepad.wButtons & OGXINPUT_GAMEPAD_BACK);
    XInput.setButton(XInputControl::BUTTON_START, gamepad.wButtons & OGXINPUT_GAMEPAD_START);

    XInput.setButton(XInputControl::BUTTON_LOGO, (gamepad.wButtons & OGXINPUT_GAMEPAD_BACK) && (gamepad.wButtons & OGXINPUT_GAMEPAD_START));

    XInput.send();
}