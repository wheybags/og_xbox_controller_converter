#pragma once
#include <Arduino.h>
#include "USBHost_t36/USBHost_t36.h"


// https://github.com/emoose/Xb2XInput/blob/8f4187a23ecd834961151fb68b7a17334820986b/Xb2XInput/XboxController.hpp#L44
struct OGXINPUT_GAMEPAD
{
  uint16_t    wButtons;
  uint8_t    bAnalogButtons[8];
  short   sThumbLX;
  short   sThumbLY;
  short   sThumbRX;
  short   sThumbRY;
};

struct XboxInputReport {
  uint8_t bReportId;
  uint8_t bSize;

  OGXINPUT_GAMEPAD Gamepad;
};


class XboxControllerDriver : public USBDriver {
public:
	XboxControllerDriver(USBHost &host) { init(); }
	XboxControllerDriver(USBHost *host) { init(); }
  virtual ~XboxControllerDriver() = default;

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

private:
	Pipe_t mypipes[3] __attribute__ ((aligned(32)));
	Transfer_t mytransfers[7] __attribute__ ((aligned(32)));
	strbuf_t mystring_bufs[1];
};