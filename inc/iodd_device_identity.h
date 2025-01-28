#ifndef IODD_DEVICE_IDENTITY_H
#define IODD_DEVICE_IDENTITY_H

#include "types.h"
#include "ezxml.h"

struct IODD_DEVICE_VARIANT_T {
    IODDStr productId;
    IODDStr nameTextId;
};

struct IODD_DEVICE_IDENTITY_T {
    int deviceId;
    int vendorId;
    IODDStr vendorName;
    IODDStr vendorTextId;
    IODDStr deviceNameTextId;
    IODDStr deviceFamilyTextId;
    std::vector<IODD_DEVICE_VARIANT_T> deviceVariants;

    void Init() {
        deviceId = 0;
        vendorId = 0;
        vendorName = "";
        vendorTextId = "";
        deviceNameTextId = "";
        deviceFamilyTextId = "";
        deviceVariants.clear();
    }

    void PrintDebug() {
        IODD_DEBUG("\tDevice ID:      %d", deviceId);
        IODD_DEBUG("\tVendor ID:      %d", vendorId);
        IODD_DEBUG("\tVendor Name:    %s", vendorName.c_str());
        IODD_DEBUG("\tVendor Text:    %s", vendorTextId.c_str());
        IODD_DEBUG("\tDevice Family:  %s", deviceFamilyTextId.c_str());
        IODD_DEBUG("\tDevice Variant: %zu", deviceVariants.size());
        for (const auto& variant : deviceVariants) {
            IODD_DEBUG("\t\tProduct ID:       %s", variant.productId.c_str());
        }
    }
};

class IODD_DeviceIdentityParser{
private:
    ezxml_t _deviceIdentityNode;

public:
    IODD_DeviceIdentityParser(ezxml_t deviceIdentityNode) : _deviceIdentityNode(deviceIdentityNode) {}
    ~IODD_DeviceIdentityParser() {}

    IODD_DEVICE_IDENTITY_T Parse();
};


#endif