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
  uint8_t wMaxPacketSize;
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


  int32_t currentDescriptorStartIndex = 0;
  while (currentDescriptorStartIndex < descriptorsSize)
  {
    const DescriptorBase* descriptor = (const DescriptorBase*)(descriptors + currentDescriptorStartIndex);

    if (descriptor->bDescriptorType == DescriptorType::Endpoint)
    {
      const EndpointDescriptor* endpoint = (const EndpointDescriptor*)descriptor;

      int32_t endpointNumber = endpoint->bEndpointAddress & 0b111;
      int32_t endpointDirection = (endpoint->bEndpointAddress & 0b10000000) >> 7;

      Serial.printf("Endpoint: bDescriptorType=%d, endpointNumber=%d, endpointDirection=%d, bmAttributes=%x, wMaxPacketSize=%d, bInterval=%d\n",
                    int(endpoint->bDescriptorType), endpointNumber, endpointDirection, int(endpoint->bmAttributes), int(endpoint->wMaxPacketSize), int(endpoint->bInterval));
    }

    currentDescriptorStartIndex += descriptor->bLength;
  }

  return false;
}

void XboxControllerDriver::disconnect()
{}
