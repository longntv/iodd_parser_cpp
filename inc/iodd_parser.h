#ifndef IODD_PARSER_H
#define IODD_PARSER_H

#include "ezxml.h"
#include "types.h"
#include "iodd_process_data.h"
#include "iodd_variable.h"
#include "iodd_data_type.h"
#include "iodd_device_identity.h"
#include "iodd_menu.h"

struct IODD_DATA_T
{
    IODD_DEVICE_IDENTITY_T deviceIdentity;
    DataTypeVect dataTypeCollection;
    VariableVect variableCollection;
    VariableMapPtr conditionVariableMap;
    MenuVect menuCollection;
    ProcessDataInVect processDataInCollection;

    void Init()
    {
        deviceIdentity.Init();
        dataTypeCollection.clear();
        variableCollection.clear();
        conditionVariableMap.clear();
        menuCollection.clear();
        processDataInCollection.clear();
    }

    void PrintDebug()
    {
        IODD_DEBUG(":::Device Identity");
        deviceIdentity.PrintDebug();
        for (auto &it : dataTypeCollection)
        {
            it.PrintDebug();
        }
        IODD_DEBUG(":::Menu Collection");
        for (auto &it : menuCollection)
        {
            it.PrintDebug();
        }
        IODD_DEBUG(":::Variable Collection");
        for (auto &it : processDataInCollection)
        {
            it.PrintDebug();
        }
        for (const auto &pair : conditionVariableMap)
        {
            IODD_DEBUG("Condition Variable ID:    %s", pair.first.c_str());
            pair.second->PrintDebug();
        }
    }
};

typedef std::unique_ptr<IODD_DATA_T>        IODD_DATA_PTR;

class IODD_Parser
{
private:
    /* data */
    ezxml_t root;
    IODD_DATA_T* _ioddData;

    IODD_DeviceIdentityParser *_deviceIdentityParser;
    IODD_DataTypeParser *_dataTypeParser;
    IODD_VariableParser *_variableParser;
    IODD_MenuParser *_menuParser;
    IODD_ProcessDataParser *_processDataParser;
    void StoreToRedis();
public:
    IODD_Parser(IODD_DATA_T* ioddData, ezxml_t root)
    {
        // TAG route:
        //    IODevice > ProfileBody > DeviceFunction > DatatypeCollection > Datatype
        //                                           > VariableCollection > Variable > Condition
        //                                           > ProcessDataCollection > ProcessData > ProcessDataIn > Datatype > RecordItem
        //                                           > UserInterface > ObserverRoleMenuSet > ObservationMenu > MenuCollection > Menu > RecordItemRef
        //                                                           > MenuCollection > Menu > MenuRef > RecordItemRef
        this->_ioddData = ioddData; // for reference result
        this->root = root;
        auto profileBodyNode = ezxml_child(root, "ProfileBody");
        auto deviceIdentityNode = ezxml_child(profileBodyNode, "DeviceIdentity");
        auto deviceFunctionNode = ezxml_child(profileBodyNode, "DeviceFunction");
        auto dataTypeCollection = ezxml_child(deviceFunctionNode, "DatatypeCollection");
        auto variableCollection = ezxml_child(deviceFunctionNode, "VariableCollection");
        auto userInterface = ezxml_child(deviceFunctionNode, "UserInterface");
        auto processDataCollection = ezxml_child(deviceFunctionNode, "ProcessDataCollection");
        auto processDataRefCollection = ezxml_child(userInterface, "ProcessDataRefCollection");

        _deviceIdentityParser = new IODD_DeviceIdentityParser(deviceIdentityNode);
        _dataTypeParser = new IODD_DataTypeParser(dataTypeCollection);
        _variableParser = new IODD_VariableParser(&_ioddData->dataTypeCollection, variableCollection);
        _menuParser = new IODD_MenuParser(userInterface);
        _processDataParser = new IODD_ProcessDataParser(&_ioddData->dataTypeCollection, processDataCollection, processDataRefCollection);
    };
    ~IODD_Parser()
    {
        if (_deviceIdentityParser)
        {
            delete _deviceIdentityParser;
        }
        if (_dataTypeParser)
        {
            delete _dataTypeParser;
        }
        if (_variableParser)
        {
            delete _variableParser;
        }
        if (_menuParser)
        {
            delete _menuParser;
        }
        if (_processDataParser)
        {
            delete _processDataParser;
        }
    };
    void Parse();
};

#endif