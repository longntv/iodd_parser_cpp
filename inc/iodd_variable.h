#ifndef IODD_VARIABLE_H
#define IODD_VARIABLE_H

#include "ezxml.h"
#include "types.h"
#include "iodd_data_type.h"

enum class VariableStatus
{
    Pending,
    Processing,
    Processed,
    Error
};

#define STATUS_PENDING 0
#define STATUS_PROCESSING 1
#define STATUS_PROCESSED 2
#define STATUS_ERROR 3

struct SingleValue
{
    IODDStr value;
    IODDStr textId;

    void PrintDebug()
    {
        IODD_DEBUG("---->SingleValue");
        IODD_DEBUG("            SingleValue::textId:    %s", textId.c_str());
        IODD_DEBUG("            SingleValue::value:     %s", value.c_str());
        IODD_DEBUG("////////////////////////////////////////////////////");
    }
};

typedef std::vector<SingleValue> SingleValueVect;

// Struct for RecordItem
struct IODD_VAR_RECORD_ITEM_T
{
    int subIndex;
    int bitOffset;
    IODDStr datatypeType;
    int bitLength;
    int lowerValue;
    int upperValue;
    IODDStr nameTextId;
    IODDStr defaultValue;
    // read from sensor
    IODDStr result;

    IODD_VAR_RECORD_ITEM_T()
    {
        subIndex = 0;
        bitOffset = 0;
        bitLength = 0;
        lowerValue = 0;
        upperValue = 0;
    }

    void PrintDebug()
    {
        IODD_DEBUG("---->RecordItem");
        IODD_DEBUG("                        IODD_VAR_RECORD_ITEM_T::subIndex:       %d", subIndex);
        IODD_DEBUG("                        IODD_VAR_RECORD_ITEM_T::bitOffset:      %d", bitOffset);
        IODD_DEBUG("                        IODD_VAR_RECORD_ITEM_T::datatypeType:   %s", datatypeType.c_str());
        IODD_DEBUG("                        IODD_VAR_RECORD_ITEM_T::bitLength:      %d", bitLength);
        IODD_DEBUG("                        IODD_VAR_RECORD_ITEM_T::lowerValue:     %d", lowerValue);
        IODD_DEBUG("                        IODD_VAR_RECORD_ITEM_T::upperValue:     %d", upperValue);
        IODD_DEBUG("                        IODD_VAR_RECORD_ITEM_T::nameTextId:     %s", nameTextId.c_str());
        IODD_DEBUG("                        IODD_VAR_RECORD_ITEM_T::defaultValue:   %s", defaultValue.c_str());
        IODD_DEBUG("////////////////////////////////////////////////////");
    }
};
typedef std::vector<IODD_VAR_RECORD_ITEM_T> VarRecordItemVect;

// Struct for Datatype
struct IODD_VAR_DATA_TYPE_T
{
    IODDStr type;
    int bitLength;
    SingleValueVect singleValues;
    VarRecordItemVect recordItems;
    
    IODDStr result;

    IODD_VAR_DATA_TYPE_T()
    {
        bitLength = 0;
    }

    void PrintDebug()
    {
        IODD_DEBUG("\t\tDatatype_T");
        IODD_DEBUG("\t\t\tIODD_VAR_DATA_TYPE_T::type:           %s", type.c_str());
        IODD_DEBUG("\t\t\tIODD_VAR_DATA_TYPE_T::bitLength:      %d", bitLength);
        IODD_DEBUG("\t\t\tIODD_VAR_DATA_TYPE_T::singleValues:   %zu", singleValues.size());
        IODD_DEBUG("\t\t\tIODD_VAR_DATA_TYPE_T::recordItems:    %zu", recordItems.size());
        IODD_DEBUG("\t\t\tIODD_VAR_DATA_TYPE_T::result:         %s", result.c_str());
        for (auto recordItem : recordItems)
        {
            recordItem.PrintDebug();
        }
        for (auto singleValue : singleValues)
        {
            singleValue.PrintDebug();
        }
    }
};
typedef std::vector<IODD_VAR_DATA_TYPE_T> IODD_DataTypeVect;

// Struct for Variable
struct IODD_VARIABLE_T
{
    IODDStr id;
    IODDStr accessRights;
    int index;
    IODDStr nameTextId;
    IODDStr defaultValue;
    IODD_VAR_DATA_TYPE_T datatype;

    IODD_VARIABLE_T()
    {
        id = "";
        index = 0;
        accessRights = "";
        nameTextId = "";
    }

    void PrintDebug()
    {
        IODD_DEBUG("::::Variable");
        IODD_DEBUG("\t\tIODD_VARIABLE_T::ID:             %s", id.c_str());
        IODD_DEBUG("\t\tIODD_VARIABLE_T::index:          %d", index);
        IODD_DEBUG("\t\tIODD_VARIABLE_T::accessRights:   %s", accessRights.c_str());
        IODD_DEBUG("\t\tIODD_VARIABLE_T::nameTextId:     %s", nameTextId.c_str());
        IODD_DEBUG("\t\tIODD_VARIABLE_T::defaultValue:   %s", defaultValue.c_str());
        IODD_DEBUG("////////////////////////////////////////////////////");
        datatype.PrintDebug();
    }
};

typedef std::vector<IODD_VARIABLE_T> VariableVect;
typedef std::map<IODDStr, IODD_VARIABLE_T *> VariableMap;
typedef std::map<IODDStr, std::shared_ptr<IODD_VARIABLE_T>> VariableMapPtr;
typedef std::shared_ptr<IODD_VARIABLE_T> IODD_VARIABLE_PTR;

class IODD_VariableParser
{
private:
    ezxml_t _variableCollectionNode;
    DataTypeVect *_dataTypeCollection;
    VarRecordItemVect ParseRecordItems(ezxml_t datatypeNode);
    SingleValueVect ParseSingleValues(ezxml_t datatypeNode);
    IODD_VAR_DATA_TYPE_T ParseDatatype(ezxml_t datatypeNode);
    IODD_VAR_DATA_TYPE_T ParseDatatypeRef(ezxml_t datatypeRefNode);
public:
    IODD_VariableParser(DataTypeVect *dataTypeCollection, ezxml_t variableCollectionNode)
    {
        _dataTypeCollection = dataTypeCollection;
        _variableCollectionNode = variableCollectionNode;
    };
    ~IODD_VariableParser() {};

    VariableVect Parse();
};
#endif