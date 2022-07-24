#include "XboxControllerDriver.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define release_assert(cond) \
    do \
    { \
        if (!(cond)) { \
          Serial.print("ASSERTION FAILED: (" #cond ") in " __FILE__ ":" TOSTRING(__LINE__)); \
          Serial.flush(); \
          while(1) {} \
        } \
    } while (0)


enum class DescriptorType : uint8_t
{
  Device = 1,
  Configuration,
  String,
  Interface,
  Endpoint,
  DeviceQualifier,
  OtherSpeedConfiguration,
  InterfacePower,
};

static void printHex(const void *ptr, uint32_t len)
{
	if (ptr == NULL || len == 0) return;
	const uint8_t *p = (const uint8_t *)ptr;
	do {
		if (*p < 16) Serial.print('0');
		Serial.print(*p++, HEX);
		Serial.print(' ');
	} while (--len);
	Serial.println();
}


struct DescriptorBase
{
  uint8_t bLength;
  DescriptorType bDescriptorType;
};

struct EndpointDescriptor
{
  uint8_t bLength;
  DescriptorType bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
};

void XboxControllerDriver::init()
{
  // This boilerplate is copied from the msController driver, it seems all the drivers have something like this
  contribute_Pipes(mypipes, sizeof(mypipes)/sizeof(Pipe_t));
  contribute_Transfers(mytransfers, sizeof(mytransfers)/sizeof(Transfer_t));
  contribute_String_Buffers(mystring_bufs, sizeof(mystring_bufs)/sizeof(strbuf_t));
  driver_ready_for_device(this);
}

bool XboxControllerDriver::claim(Device_t* device, int type, const uint8_t* descriptors, uint32_t descriptorsSize)
{
  // only claim at interface level
  if (type != 1)
    return false;

  // 9 = USB hub
  if (device->bDeviceClass == 9)
    return false;

//  release_assert(false);
//  release_assert(descriptors[1] == uint8_t(DescriptorType::Interface));

//  int32_t bNumEndPoints = descriptors[4];
//  constexpr int32_t endpointsStart = 9;
//  constexpr int32_t endpointDescriptorSize = 7;

  const EndpointDescriptor* endpointIn = nullptr;
  const EndpointDescriptor* endpointOut = nullptr;
  {
    int32_t currentDescriptorStartIndex = 0;
    while (currentDescriptorStartIndex < int32_t(descriptorsSize))
    {
      const DescriptorBase* descriptor = (const DescriptorBase*) (descriptors + currentDescriptorStartIndex);

      if (descriptor->bDescriptorType == DescriptorType::Endpoint)
      {
        const EndpointDescriptor* endpoint = (const EndpointDescriptor*) descriptor;

        int32_t endpointNumber = endpoint->bEndpointAddress & 0b111;
        int32_t endpointDirection = (endpoint->bEndpointAddress & 0b10000000) >> 7;

        Serial.printf("Endpoint: bDescriptorType=%d, endpointNumber=%d, endpointDirection=%d, bmAttributes=%x, wMaxPacketSize=%d, bInterval=%d\n",
                      int(endpoint->bDescriptorType), endpointNumber, endpointDirection, int(endpoint->bmAttributes), int(endpoint->wMaxPacketSize),
                      int(endpoint->bInterval));

        if (endpoint->bmAttributes != 0b11) // not an "Interrupt" endpoint
          continue;

        if (endpointDirection)
        {
          release_assert(!endpointIn);
          endpointIn = endpoint;
        }
        else
        {
          release_assert(!endpointOut);
          endpointOut = endpoint;
        }
      }

      currentDescriptorStartIndex += descriptor->bLength;
    }
  }

  release_assert(endpointIn && endpointOut);

  this->pipeIn = new_Pipe(device, 3, endpointIn->bEndpointAddress, 1, endpointIn->wMaxPacketSize, endpointIn->bInterval);
  this->pipeIn->callback_function = pipeInCallbackStatic;
  this->pipeOut = new_Pipe(device, 3, endpointIn->bEndpointAddress, 0, endpointIn->wMaxPacketSize, endpointIn->bInterval);
  this->pipeOut->callback_function = pipeOutCallbackStatic;

  memset(&this->reportBuffer, 0, sizeof(XboxInputReport));
  queue_Data_Transfer(this->pipeIn, &this->reportBuffer, sizeof(XboxInputReport), this);

  return true;
}

void XboxControllerDriver::disconnect()
{}

void XboxControllerDriver::pipeInCallbackStatic(const Transfer_t* transfer)
{
  Serial.print("XboxControllerDriver::pipeInCallbackStatic\n");
  if (transfer->driver)
    ((XboxControllerDriver*)transfer->driver)->pipeInCallback(transfer);
}

void XboxControllerDriver::pipeOutCallbackStatic(const Transfer_t* transfer)
{
  Serial.print("XboxControllerDriver::pipeOutCallbackStatic\n");
  if (transfer->driver)
    ((XboxControllerDriver*)transfer->driver)->pipeOutCallback(transfer);
}

void XboxControllerDriver::pipeInCallback(const Transfer_t* transfer)
{
  Serial.print("XboxControllerDriver::pipeInCallback\n");
  printHex(transfer->buffer, transfer->length);
  Serial.print("\n-----\n");

  memset(&this->reportBuffer, 0, sizeof(XboxInputReport));
  queue_Data_Transfer(this->pipeIn, &this->reportBuffer, sizeof(XboxInputReport), this);
}

void XboxControllerDriver::pipeOutCallback(const Transfer_t* transfer)
{
  Serial.print("XboxControllerDriver::pipeOutCallback\n");
  printHex(transfer->buffer, transfer->length);
  Serial.print("\n-----\n");
}

void XboxControllerDriver::control(const Transfer_t* transfer)
{
  Serial.print("XboxControllerDriver::control\n");
  printHex(transfer->buffer, transfer->length);
  Serial.print("\n-----\n");
}
