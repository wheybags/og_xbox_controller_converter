#pragma once
#include <Arduino.h>
#include <USBHost_t36.h>


// https://github.com/emoose/Xb2XInput/blob/8f4187a23ecd834961151fb68b7a17334820986b/Xb2XInput/XboxController.hpp#L44

// digital button bitmasks
#define OGXINPUT_GAMEPAD_DPAD_UP           0x0001
#define OGXINPUT_GAMEPAD_DPAD_DOWN         0x0002
#define OGXINPUT_GAMEPAD_DPAD_LEFT         0x0004
#define OGXINPUT_GAMEPAD_DPAD_RIGHT        0x0008
#define OGXINPUT_GAMEPAD_START             0x0010
#define OGXINPUT_GAMEPAD_BACK              0x0020
#define OGXINPUT_GAMEPAD_LEFT_THUMB        0x0040
#define OGXINPUT_GAMEPAD_RIGHT_THUMB       0x0080

// analog button indexes
#define OGXINPUT_GAMEPAD_A                0
#define OGXINPUT_GAMEPAD_B                1
#define OGXINPUT_GAMEPAD_X                2
#define OGXINPUT_GAMEPAD_Y                3
#define OGXINPUT_GAMEPAD_BLACK            4
#define OGXINPUT_GAMEPAD_WHITE            5
#define OGXINPUT_GAMEPAD_LEFT_TRIGGER     6
#define OGXINPUT_GAMEPAD_RIGHT_TRIGGER    7

#pragma pack(push, 1)
struct OGXINPUT_RUMBLE
{
  uint16_t wLeftMotorSpeed = 0;
  uint16_t wRightMotorSpeed = 0;
};

struct OGXINPUT_GAMEPAD
{
  uint16_t wButtons;
  uint8_t bAnalogButtons[8];
  int16_t sThumbLX;
  int16_t sThumbLY;
  int16_t sThumbRX;
  int16_t sThumbRY;
};

struct XboxInputReport {
  uint8_t bReportId;
  uint8_t bSize;

  OGXINPUT_GAMEPAD Gamepad;
};

struct XboxOutputReport {
  uint8_t bReportId;
  uint8_t bSize;
  OGXINPUT_RUMBLE Rumble;
};
#pragma pack(pop)

class XboxControllerDriver : public USBDriver {
public:
	XboxControllerDriver(USBHost &host) { init(); }
	XboxControllerDriver(USBHost *host) { init(); }
  virtual ~XboxControllerDriver() = default;

  OGXINPUT_GAMEPAD getLatestReport() const { return reportBuffer.Gamepad; }
  void setRumble(OGXINPUT_RUMBLE rumble, bool force = false);

protected:
	bool claim(Device_t *device, int type, const uint8_t *descriptors, uint32_t len) override;
	void disconnect() override;
	void init();

  void control(const Transfer_t *transfer) override;

  static void pipeInCallbackStatic(const Transfer_t* transfer);
  static void pipeOutCallbackStatic(const Transfer_t* transfer);

  void pipeInCallback(const Transfer_t *transfer);
  void pipeOutCallback(const Transfer_t* transfer);

private:
  Pipe_t* pipeIn = nullptr;
  Pipe_t* pipeOut = nullptr;

  XboxInputReport reportBuffer;

  XboxOutputReport rumbleBuffer;

private:
	Pipe_t mypipes[3] __attribute__ ((aligned(32)));
	Transfer_t mytransfers[7] __attribute__ ((aligned(32)));
	strbuf_t mystring_bufs[1];
};