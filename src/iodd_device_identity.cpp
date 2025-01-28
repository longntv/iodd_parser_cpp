#include "iodd_device_identity.h"
#include "iodd_helper.h"

IODD_DEVICE_IDENTITY_T IODD_DeviceIdentityParser::Parse() {
    IODD_DEVICE_IDENTITY_T deviceIdentity;

    XMLGetNumeric(_deviceIdentityNode,  "deviceId", deviceIdentity.deviceId, T_INT);
    XMLGetNumeric(_deviceIdentityNode,  "vendorId", deviceIdentity.vendorId, T_INT);
    XMLGetStr(_deviceIdentityNode,      "vendorName", deviceIdentity.vendorName);

    // Parse child nodes
    auto vendorTextNode = ezxml_child(_deviceIdentityNode, "VendorText");
    if (vendorTextNode) {
        XMLGetStr(vendorTextNode, "textId", deviceIdentity.vendorTextId);
    }

    auto deviceNameNode = ezxml_child(_deviceIdentityNode, "DeviceName");
    if (deviceNameNode) {
        XMLGetStr(deviceNameNode, "textId", deviceIdentity.deviceNameTextId);
    }

    auto deviceFamilyNode = ezxml_child(_deviceIdentityNode, "DeviceFamily");
    if (deviceFamilyNode) {
        XMLGetStr(deviceFamilyNode, "textId", deviceIdentity.deviceFamilyTextId);
    }

    // Parse device variants
    ezxml_t deviceVariantCollectionNode = ezxml_child(_deviceIdentityNode, "DeviceVariantCollection");
    if (deviceVariantCollectionNode) {
        for (ezxml_t variantNode = ezxml_child(deviceVariantCollectionNode, "DeviceVariant"); variantNode; variantNode = variantNode->next) {
            IODD_DEVICE_VARIANT_T variant;
            XMLGetStr(variantNode, "productId",     variant.productId);
            ezxml_t nameNode = ezxml_child(variantNode, "Name");
            if (nameNode) {
                XMLGetStr(nameNode, "textId", variant.nameTextId);
            }
            deviceIdentity.deviceVariants.push_back(variant);
        }
    }
    return deviceIdentity;
}