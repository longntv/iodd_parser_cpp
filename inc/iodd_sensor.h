#ifndef IODD_SENSOR_H
#define IODD_SENSOR_H

#include <queue>

#include "types.h"
#include "iodd_parser.h"

struct MODEL_IODD_RECORD_T
{
    IODDStr _productId;
    int    _vendorId;
    int    _deviceId;
    IODDStr _recordId;
    IODDStr _textId;
    int    _bitLength;
    int    _bitOffset;
    int    _subindex;
    IODDStr _type;
    IODDStr _min;
    IODDStr _max;
    //     Menu         Observation
    IODDStr _displayFormat;
    float   _gradient;
    int    _offset;
    int    _unitCode;

    float  currentValue;
    IODDStr valueInFormat;

    MODEL_IODD_RECORD_T()
    {
        _productId = "";
        _vendorId = 0;
        _deviceId = 0;
        _recordId = "";
        _textId = "";
        _bitLength = 0;
        _bitOffset = 0;
        _subindex = 0;
        _type = "";
        _min = "";
        _max = "";
        _displayFormat = "";
        _gradient = 0;
        _offset = 0;
        _unitCode = 0;

        currentValue = 0;
        valueInFormat = "";
    }
    void PrintDebug()
    {
        IODD_INFO("\tSensor Record");
        IODD_INFO("\t\t_recordId:      %s", _recordId.c_str());
        IODD_INFO("\t\t_textId:        %s", _textId.c_str());
        IODD_INFO("\t\t_bitLength:     %d", _bitLength);
        IODD_INFO("\t\t_bitOffset:     %d", _bitOffset);
        IODD_INFO("\t\t_subindex:      %d", _subindex);
        IODD_INFO("\t\t_type:          %s", _type.c_str());
        IODD_INFO("\t\t_min:           %s", _min.c_str());
        IODD_INFO("\t\t_max:           %s", _max.c_str());
        IODD_INFO("\t\t_displayFormat: %s", _displayFormat.c_str());
        IODD_INFO("\t\t_gradient:      %f", _gradient);
        IODD_INFO("\t\t_offset:        %d", _offset);
        IODD_INFO("\t\t_unitCode:      %d", _unitCode);
    }
};
typedef std::vector<MODEL_IODD_RECORD_T> IODDRecordVect;

struct MODEL_SENSOR_T
{
    int         _deviceId;
    int         _vendorId;

    IODDStr      _productId;
    IODDStr      _vendorName;

    IODDRecordVect records;
    IODD_DATA_T ioddData;

    MODEL_SENSOR_T() {
        _productId      = "";
        _deviceId       = 0;
        _vendorId       = 0;
        _vendorName     = "";
    }
    void PrintDebug() {
        IODD_INFO("SENSOR INFO");
        IODD_INFO("\tdeviceId:       %d", _deviceId);
        IODD_INFO("\tvendorId:       %d", _vendorId);
        IODD_INFO("\tproductId:      %s", _productId.c_str());
        IODD_INFO("\tvendorName:     %s", _vendorName.c_str());
        IODD_INFO("\ttotal records:  %zu", records.size());
        for(auto &it : records) {
            it.PrintDebug();
        }
    }
};
typedef std::vector<MODEL_SENSOR_T>               IODDSensorVect;

#define WAIT_FOR_READ_REG_SENSOR_TIMEOUT 1

class IODD_Sensor
{
private:
    MODEL_SENSOR_T  _sensor;
    IODDCondVar      _processingCondVar;
    IODDMutex        _processingMutex;
    IODDStr         _receivedValue;
    IODDAtomicBool   _isProcessing{false};

    bool parseIoddFile(IODDStr filepath, IODD_DATA_T* ioddData);
    void processProcessDataIn(const IODD_PROCESS_DATA_T& processDataItem);
    void processRecordItems(const IODD_PROCESS_DATA_T& processDataItem);
    void processSimpleRecord(const IODD_PROCESS_DATA_T& processDataItem);
    void processMenuCollection(MODEL_IODD_RECORD_T& record,
                             const IODD_PROCESS_DATA_T& processDataItem,
                             int subindex);
    bool processMenuRecords(const IODD_MENU_T& menu,
                       const IODD_PROCESS_DATA_T& processDataItem,
                       MODEL_IODD_RECORD_T& record,
                       int subindex);               
    bool isMatchingMenuRecord(const IODD_MENU_RECORD_T& menuRecord,
                         const IODD_PROCESS_DATA_T& processDataItem,
                         const IODD_MENU_T& menu,
                         int subindex);
    bool processConditionalMenu(
                          const IODD_DATA_T& ioddData,
                          const IODD_MENU_T& menu,
                          const IODD_PROCESS_DATA_T& processDataItem,
                          MODEL_IODD_RECORD_T& record,
                          int subindex);                         
public:
    IODD_Sensor();
    ~IODD_Sensor();

    void ProcessRegValue(IODDStr regStr, IODDStr value);
    bool ProcessValue(IODDStr value);
    void FormatDisplayValue(MODEL_IODD_RECORD_T* record);
    void LoadSensor(IODDStr vendId, IODDStr devId, IODDStr name, IODDStr ioddFile);
};

#endif