#include <Arduino.h>
#include <USBHost_t36.h>
#include <XInput.h>
#include "XboxControllerDriver.hpp"

USBHost usbHost;
// For some reason my xbox controller actually presents as a hub with a controller device attached.
// Left a second one in here too because why not.
[[maybe_unused]] USBHub hub1(usbHost);
[[maybe_unused]] USBHub hub2(usbHost);
XboxControllerDriver xboxControllerDriver(usbHost);

void mainSetup()
{
  while (!Serial);

  USBHost::begin();

  XInput.begin();
  XInput.setAutoSend(false);
  XInput.setJoystickRange(-32768, 32767);
  XInput.setTriggerRange(0, 255);
}

void mainLoop()
{
  USBHost::Task();

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

  auto scale = [](uint8_t x360Rumble) { return uint16_t(x360Rumble * 257);};
  xboxControllerDriver.setRumble(OGXINPUT_RUMBLE{
    .wLeftMotorSpeed = scale(XInput.getRumbleLeft()),
    .wRightMotorSpeed = scale(XInput.getRumbleRight())
  });

  xboxControllerDriver.update();
  XInput.send();
}