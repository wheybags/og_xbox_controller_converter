#pragma once
#include <Arduino.h>
#include "USBHost_t36/USBHost_t36.h"

class XboxControllerDriver : public USBDriver {
public:
	XboxControllerDriver(USBHost &host) { init(); }
	XboxControllerDriver(USBHost *host) { init(); }
  virtual ~XboxControllerDriver() = default;

protected:
	bool claim(Device_t *device, int type, const uint8_t *descriptors, uint32_t len) override;
	void disconnect() override;
	void init();
private:
	Pipe_t mypipes[3] __attribute__ ((aligned(32)));
	Transfer_t mytransfers[7] __attribute__ ((aligned(32)));
	strbuf_t mystring_bufs[1];
};