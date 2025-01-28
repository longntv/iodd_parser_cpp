// #include <bitset>
// #include <string>
// #include <sstream>
// #include <iomanip>

#include "iodd_sensor.h"
#include "types.h"
#include "iodd_helper.h"

IODD_Sensor::IODD_Sensor(){}

IODD_Sensor::~IODD_Sensor(){}

bool IODD_Sensor::parseIoddFile(IODDStr filepath, IODD_DATA_T* ioddData)
{
    ezxml_t ioddDeviceRootTag = ezxml_parse_file(filepath.c_str());
    if(ioddDeviceRootTag == NULL) {
        IODD_WARN("Failed to parse xml file");
        return false;
    }
    IODD_Parser ioddParser(ioddData, ioddDeviceRootTag);
    ioddParser.Parse();
    return true;
}

void IODD_Sensor::processProcessDataIn(const IODD_PROCESS_DATA_T& processDataItem) {
    if (processDataItem.processDataIn.datatype.recordItems.size() > 0) {
        processRecordItems(processDataItem);
    } else {
        processSimpleRecord(processDataItem);
    }
}

void IODD_Sensor::processRecordItems(const IODD_PROCESS_DATA_T& processDataItem) {
    IODD_INFO("ProcessDataIn %s has %zu records", processDataItem.id.c_str(), 
             processDataItem.processDataIn.datatype.recordItems.size());
             
    for (const auto& recordItem : processDataItem.processDataIn.datatype.recordItems) {
        MODEL_IODD_RECORD_T record;
        record._recordId = processDataItem.id;
        record._textId = recordItem.nameTextId;
        record._bitLength = recordItem.bitLength;
        record._bitOffset = recordItem.bitOffset;
        record._min = recordItem.lowerValue;
        record._max = recordItem.upperValue;
        record._type = recordItem.type;
        record._unitCode = STR2INT(recordItem.unitCode);
        record._displayFormat = recordItem.displayFormat;
        record._gradient = recordItem.gradient;
        record._offset = recordItem.offset;
        record._subindex = recordItem.subindex;

        processMenuCollection(record, processDataItem, recordItem.subindex);
        _sensor.records.push_back(record);
    }
}

void IODD_Sensor::processSimpleRecord(const IODD_PROCESS_DATA_T& processDataItem) {
    MODEL_IODD_RECORD_T record;
    record._textId = processDataItem.id;
    record._type = processDataItem.processDataIn.datatype.type;
    record._bitLength = processDataItem.processDataIn.datatype.bitLength;
    
    processMenuCollection(record, processDataItem, 0);  // 0 as default subindex
    _sensor.records.push_back(record);
}

void IODD_Sensor::processMenuCollection(MODEL_IODD_RECORD_T& record,
                         const IODD_PROCESS_DATA_T& processDataItem,
                         int subindex) {
    for (const auto& menu : _sensor.ioddData.menuCollection) {
        IODD_DEBUG("Menu %s condition %s %s", menu.menuId.c_str() , menu.condition.first.c_str(), menu.condition.second.c_str());
        if (!menu.condition.first.empty()) {
            if (processConditionalMenu(_sensor.ioddData, menu, processDataItem, record, subindex)) {
                IODD_WARN("found conditional menu %s", menu.menuId.c_str());
                break;
            }
        } else {
            processMenuRecords(menu, processDataItem, record, subindex);
        }
    }
}

bool IODD_Sensor::processConditionalMenu(const IODD_DATA_T& ioddData,
                          const IODD_MENU_T& menu,
                          const IODD_PROCESS_DATA_T& processDataItem,
                          MODEL_IODD_RECORD_T& record,
                          int subindex) {
    auto var = ioddData.conditionVariableMap.find(menu.condition.first);
    if (var == ioddData.conditionVariableMap.end()) {
        return false;
    }

    IODD_INFO("Variable %s has value %s", var->first.c_str(), var->second->datatype.result.c_str());
    
    if (var->second->datatype.result != menu.condition.second) {
        return false;
    }

    IODD_WARN("found menu %s", menu.menuId.c_str());
    return processMenuRecords(menu, processDataItem, record, subindex);
}

bool IODD_Sensor::processMenuRecords(const IODD_MENU_T& menu,
                       const IODD_PROCESS_DATA_T& processDataItem,
                       MODEL_IODD_RECORD_T& record,
                       int subindex) 
{
    for (const auto& menuRecord : menu.menuRecords) {
        if (isMatchingMenuRecord(menuRecord, processDataItem, menu, subindex)) {
            record._subindex = menuRecord.subIndex;
            record._gradient = menuRecord.gradient;
            record._offset = menuRecord.offset;
            record._unitCode = menuRecord.unitCode;
            record._displayFormat = menuRecord.displayFormat;
            return true;
        }
    }
    IODD_WARN("not found menu-record");
    return false;
}

bool IODD_Sensor::isMatchingMenuRecord(const IODD_MENU_RECORD_T& menuRecord,
                         const IODD_PROCESS_DATA_T& processDataItem,
                         const IODD_MENU_T& menu,
                         int subindex) 
{
    bool isMatchingVariable = 
        (menuRecord.variableId == "V_ProcessDataInput" && processDataItem.processDataIn.id == "V_PdInT") ||
        (menuRecord.variableId == processDataItem.id) ||
        (menu.nameId == processDataItem.processDataIn.nameTextId);
        
    return isMatchingVariable && (menuRecord.subIndex == subindex);
}

bool IODD_Sensor::ProcessValue(IODDStr value)
{
    bool ret = false;    
    IODD_INFO("ProcessValue:        %s", value.c_str());
    for(int idx = 0; idx < _sensor.records.size(); idx++) {
        MODEL_IODD_RECORD_T* record = &(_sensor.records.at(idx));
        ret = IODD_Helper_ParseRecordValue(record->_type, record->_bitLength, record->_bitOffset, value, record->currentValue);
        if(ret == false) return false;
        FormatDisplayValue(record);
        IODD_INFO("Record %s: %s", record->_textId.c_str(), record->valueInFormat.c_str());
    }
    return ret;
}

void IODD_Sensor::FormatDisplayValue(MODEL_IODD_RECORD_T* record)
{
    if(record->_type == RECORD_TYPE_BOOL) {
        record->valueInFormat = record->currentValue == 0? "False" : "True";
    } else if(  record->_type == RECORD_TYPE_INT ){
        float value = record->currentValue * record->_gradient + record->_offset;
        if(record->_displayFormat == "") {
            record->_displayFormat = "Dec";
        }
        record->valueInFormat = IODD_Helper_FormatDisplay(value, record->_displayFormat);
    } else if( record->_type == RECORD_TYPE_FLOAT ){
        float value = record->currentValue * record->_gradient + record->_offset;
        if(record->_displayFormat == "" || record->_displayFormat == "Dec") {
            record->_displayFormat = "Dec.6";
        }
        record->valueInFormat = IODD_Helper_FormatDisplay(value, record->_displayFormat);
    } else if(  record->_type == RECORD_TYPE_UINT ) {
        u32 valueInt = record->currentValue * record->_gradient + record->_offset;
        if(record->_displayFormat == "") {
            record->_displayFormat = "Dec";
        }
        record->valueInFormat = IODD_Helper_FormatDisplay(valueInt, record->_displayFormat);
    }
}

void IODD_Sensor::ProcessRegValue(IODDStr regStr, IODDStr value)
{
    int reg = STR2INT(regStr);

    switch (reg)
    {
        case PRODUCT_ID:{
            IODD_WARN("product-id: %s", value.c_str());
            IODDStr productID = IODD_Helper_ParseStringValue(value);
            _sensor._productId = productID;
            _processingCondVar.notify_one();
            break;
        }
        case SERIAL_NUMBER:
            IODD_WARN("Serial Number: %s", value.c_str());
            break;
        case FW_REV:
            IODD_WARN("FWR: %s", value.c_str());
            break;
        case HW_REV:
            IODD_WARN("HWR: %s", value.c_str());
            break;
        default:
        {
            _receivedValue = value;
            _isProcessing = false;
            _processingCondVar.notify_one();            
            break;
        }
    }
}
void IODD_Sensor::LoadSensor(IODDStr vendId, IODDStr devId, IODDStr name, IODDStr ioddFile)
{
    IODD_INFO("LoadSensor: %s %s %s", name.c_str(), vendId.c_str(), devId.c_str());
    IODD_INFO("IODD Spec : %s", ioddFile.c_str());

    if (parseIoddFile(ioddFile, &_sensor.ioddData) == false) {
        IODD_WARN("Failed to parse iodd file");
        return;
    }

    IODD_DATA_T* ioddData = &_sensor.ioddData;

    // in case of device variants
    if(ioddData->deviceIdentity.deviceVariants.size() > 1) {
        _sensor._productId = "";
        IODDUniqueLock lock(_processingMutex);
        // TODO: read product ID from sensor
        // wait for until product ID is read or timeout, load the first product ID
        if (_processingCondVar.wait_for(lock, IODDSeconds(WAIT_FOR_READ_REG_SENSOR_TIMEOUT), [this] { 
                return _sensor._productId.empty() == false; 
            })) {
            IODD_INFO("Read from Sensor:: Product ID is %s", _sensor._productId.c_str());
        } else {
            IODD_WARN("Read product ID is timeout");
        }
    }

    if(_sensor._productId.empty()) {
        // Load from Device Identity::DeviceVariants::ProductID
        if(ioddData->deviceIdentity.deviceVariants.size() == 0 && (name.empty() || name == "-")) {
            IODD_WARN("LoadSensor failed. Product ID and Name is empty");
            return;
        }
        _sensor._productId = ioddData->deviceIdentity.deviceVariants.at(0).productId;
        IODD_WARN("Load as default product ID: %s", _sensor._productId.c_str());
    }
    if(name.empty() || name == "-") {
        name = ioddData->deviceIdentity.deviceVariants.at(0).productId;
    }
    IODD_INFO("Loaded IODD file successfully - %d %d", ioddData->deviceIdentity.vendorId, ioddData->deviceIdentity.deviceId);
    // create sensor based on read product ID
    _sensor._deviceId       = ioddData->deviceIdentity.deviceId;
    _sensor._vendorId       = ioddData->deviceIdentity.vendorId;
    _sensor._vendorName     = ioddData->deviceIdentity.vendorName;

    IODD_INFO("Start loading records");

    // read all variables from ioddDta->conditionVariableMap
    for(auto &it : ioddData->conditionVariableMap) {
        IODDStr varId = it.first;
        IODDUniqueLock lock(_processingMutex);
        IODD_INFO("Request variable %s ", varId.c_str());
        // TODO: read variable from sensor
        _isProcessing = true;
        _receivedValue = "";
        if (_processingCondVar.wait_for(lock, IODDSeconds(WAIT_FOR_READ_REG_SENSOR_TIMEOUT), [this] { 
                return _isProcessing == false; 
            })) {
                IODD_INFO("Read from Sensor:: Variable %s is %s", varId.c_str(), _receivedValue.c_str());
                if(it.second->datatype.recordItems.size() > 0) {
                    IODD_INFO("Variable %s has %zu records", varId.c_str(), it.second->datatype.recordItems.size());

                    for(auto &rit : it.second->datatype.recordItems) {
                        float ret = 0;
                        IODD_INFO("RecordItem %d %d %s %d", 
                                rit.subIndex, 
                                rit.bitOffset, 
                                rit.datatypeType.c_str(), 
                                rit.bitLength);

                        if(IODD_Helper_ParseRecordValue(rit.datatypeType, rit.bitLength, rit.bitOffset, _receivedValue, ret))
                        {    
                            IODD_INFO("IODD_Helper_ParseRecordValue %f", ret);
                            rit.result = TOSTR(int(ret));
                        } else {
                            IODD_WARN("IODD_Helper_ParseRecordValue failed, load default value");
                            rit.result = rit.defaultValue;
                        }
                    }
                } else {
                    IODD_INFO("Variable %s has no record", varId.c_str());
                    u32 ret = 0;
                    if(IODD_Helper_ParseRegisterValue(_receivedValue, it.second->datatype.bitLength, ret)){
                        IODD_INFO("Parse simple value %d", ret);
                        it.second->datatype.result = TOSTR(ret);
                    } else {
                        it.second->datatype.result = it.second->defaultValue;
                        IODD_WARN("Parse simple value failed, load default value %s", it.second->defaultValue.c_str());
                    }
                }

        } else {
            if(it.second->datatype.recordItems.size() > 0) {
                for(auto &rit : it.second->datatype.recordItems) {
                    rit.result = rit.defaultValue;
                }
            } else {
                it.second->datatype.result = it.second->defaultValue;
            }
            IODD_WARN("Variable %s is timeout - load default value %s", varId.c_str(), it.second->defaultValue.c_str());
        }
        it.second->PrintDebug();
    }

    // make record for sensor
    // process all ProcessDataIn
    for(auto &processDataItem: _sensor.ioddData.processDataInCollection) {
        // check process data has condition
        bool skip = false;
        IODD_INFO("ProcessDataIn::Conditions %s has %zu conditions", processDataItem.id.c_str(), processDataItem.conditions.size());
        if(processDataItem.conditions.size() > 0) {
            // check condition with variable value
            for(auto &cond : processDataItem.conditions) {
                auto var = ioddData->conditionVariableMap.find(cond.first);
                if(var != ioddData->conditionVariableMap.end()) {
                    IODD_INFO("Variable %s has value %s", var->first.c_str(), var->second->datatype.result.c_str());
                    IODD_INFO("Condition %s %d", cond.first.c_str(), cond.second);
                    if(var->second->datatype.recordItems.size() > 0) {
                        if(var->second->datatype.recordItems.at(0).result != TOSTR(cond.second)) {
                            IODD_INFO("found condition %s %d", var->first.c_str(), cond.second);
                            skip = true;   // Set flag to true
                            break;         // Exit the inner loop
                        }
                    } else {
                        IODD_INFO("Variable %s has value %s", var->first.c_str(), var->second->datatype.result.c_str());
                        if(var->second->datatype.result != TOSTR(cond.second)) {
                            IODD_INFO("found condition %s %d", var->first.c_str(), cond.second);
                            skip = true;   // Set flag to true
                            break;         // Exit the inner loop
                        }
                    }
                }
            }
        }
        if(skip) {
            IODD_WARN("Skip ProcessDataIn %s", processDataItem.id.c_str());
            continue;
        }

        processProcessDataIn(processDataItem);
    }
    _sensor.PrintDebug();
}
