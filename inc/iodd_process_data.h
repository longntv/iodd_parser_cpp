#ifndef IODD_PROCESS_DATA_H
#define IODD_PROCESS_DATA_H
#include <ezxml.h>
#include "types.h"
#include "iodd_data_type.h"

// struct for ProcessDataRef
struct IODD_PROCESS_DATA_REF_ITEM_T
{
    int subindex;
    float gradient;
    int offset;
    IODDStr unitCode;
    IODDStr displayFormat;

    // constructor
    IODD_PROCESS_DATA_REF_ITEM_T()
    {
        subindex = 0;
        gradient = 1.0;
        offset = 0;
    }

    void PrintDebug()
    {
        IODD_DEBUG("       IODD_PROCESS_DATA_REF_ITEM_T");
        IODD_DEBUG("           subindex:             %d", subindex);
        IODD_DEBUG("           subindex:             %d", subindex);
        IODD_DEBUG("           gradient:             %f", gradient);
        IODD_DEBUG("           offset:               %d", offset);
        IODD_DEBUG("           unitCode:             %s", unitCode.c_str());
        IODD_DEBUG("           displayFormat:        %s", displayFormat.c_str());
        IODD_DEBUG("---------------------------------------------------");
    }
};
typedef std::vector<IODD_PROCESS_DATA_REF_ITEM_T> ProcessDataRefItemVect;

struct IODD_PROCESS_DATA_REF_T
{
    IODDStr processDataInId;
    ProcessDataRefItemVect pdRefItems;

    void PrintDebug()
    {
        IODD_DEBUG("IODD_PROCESS_DATA_REF");
        IODD_DEBUG("   ProcessDataInId:    %s", processDataInId.c_str());
        for (auto &it : pdRefItems)
        {
            it.PrintDebug();
        }
    }
};

typedef std::vector<IODD_PROCESS_DATA_REF_T> ProcessDataRefVect;

// Struct for RecordItem
struct IODD_PD_RECORD_ITEM_T
{
    int subindex;
    int bitOffset;
    int bitLength;
    int offset;
    float gradient;
    IODDStr lowerValue;
    IODDStr upperValue;
    IODDStr type;
    IODDStr unitCode;
    IODDStr displayFormat;
    IODDStr nameTextId;

    // constructor
    IODD_PD_RECORD_ITEM_T()
    {
        subindex = 0;
        bitOffset = 0;
        bitLength = 0;
        gradient = 1.0;
        offset = 0;
    }
};
typedef std::vector<IODD_PD_RECORD_ITEM_T> ProcessDataRecordItemVect;

struct IODD_PD_DATA_TYPE_T
{
    int bitLength;
    IODDStr type;
    ProcessDataRecordItemVect recordItems;

    void PrintDebug()
    {
        IODD_DEBUG("\tIODD_PD_DATA_TYPE_T");
        IODD_DEBUG("\t\tBitLength:      %d", bitLength);
        IODD_DEBUG("\t\tType:           %s", type.c_str());
        for (auto &it : recordItems)
        {
            IODD_DEBUG("\t\t\tSubindex:       %d", it.subindex);
            IODD_DEBUG("\t\t\tBitOffset:      %d", it.bitOffset);
            IODD_DEBUG("\t\t\tType:           %s", it.type.c_str());
            IODD_DEBUG("\t\t\tBitLength:      %d", it.bitLength);
            IODD_DEBUG("\t\t\tOffset:         %d", it.offset);
            IODD_DEBUG("\t\t\tGradient:       %f", it.gradient);
            IODD_DEBUG("\t\t\tLowerValue:     %s", it.lowerValue.c_str());
            IODD_DEBUG("\t\t\tUpperValue:     %s", it.upperValue.c_str());
            IODD_DEBUG("\t\t\tUnitCode:       %s", it.unitCode.c_str());
            IODD_DEBUG("\t\t\tDisplayFormat:  %s", it.displayFormat.c_str());
            IODD_DEBUG("---------------------------------------------------");
        }
        IODD_DEBUG("---------------------------------------------------");
    }
};

// Struct for ProcessDataIn/ProcessDataOut
struct IODD_PROCESS_DATA_IO_T
{
    IODDStr id;
    int bitLength;
    IODDStr nameTextId;
    IODD_PD_DATA_TYPE_T datatype;

    void PrintDebug()
    {
        IODD_DEBUG("   IODD_PROCESS_DATA_IO_T");
        IODD_DEBUG("   ID:             %s", id.c_str());
        IODD_DEBUG("   BitLength:      %d", bitLength);
        IODD_DEBUG("   NameTextId:     %s", nameTextId.c_str());
        IODD_DEBUG("---------------------------------------------------");
        datatype.PrintDebug();
    }
};

// Struct for ProcessData
struct IODD_PROCESS_DATA_T
{
    IODDStr id;
    IODDStrIntPairVect conditions; // Variable ID and value
    IODD_PROCESS_DATA_IO_T processDataIn;

    void PrintDebug()
    {
        IODD_DEBUG("\n\nIODD_PROCESS_DATA_T");
        IODD_DEBUG("ID:             %s", id.c_str());
        for (auto &it : conditions)
        {
            IODD_DEBUG("Condition Variable ID:    %s", it.first.c_str());
            IODD_DEBUG("Condition Value:          %d", it.second);
            IODD_DEBUG("---------------------------------------------------");
        }
        processDataIn.PrintDebug();
    }
};

typedef std::vector<IODD_PROCESS_DATA_T> ProcessDataInVect;

class IODD_ProcessDataParser
{
private:
    DataTypeVect *_dataTypeCollection;
    ProcessDataRefVect _processDataRefCollection;
    ezxml_t _processDataCollectionNode;
    ezxml_t _processDataRefCollectionNode;

    ProcessDataRecordItemVect ParseRecordItems(IODDStr processDataInId, ezxml_t datatypeNode);
    IODD_PROCESS_DATA_IO_T ParseProcessDataInOut(ezxml_t processDataInOutNode);
    void ParseProcessDataRefCollection();

public:
    IODD_ProcessDataParser(DataTypeVect *dataTypeCollection,
                           ezxml_t processDataCollectionNode,
                           ezxml_t processDataRefCollectionNode)
    {
        _dataTypeCollection = dataTypeCollection;
        _processDataCollectionNode = processDataCollectionNode;
        _processDataRefCollectionNode = processDataRefCollectionNode;
    };
    ~IODD_ProcessDataParser() {};
    ProcessDataRefVect GetProcessDataRefCollection() { return _processDataRefCollection; }

    IODD_PROCESS_DATA_REF_T *FindProcessDataRefById(IODDStr id);
    ProcessDataInVect Parse();
};

#endif
