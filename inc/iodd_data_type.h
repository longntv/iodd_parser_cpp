#ifndef IODD_DATA_TYPE_H
#define IODD_DATA_TYPE_H

#include "ezxml.h"
#include "types.h"

struct IODD_DT_RECORD_ITEM_T
{
    int subIndex;
    int bitOffset;
    IODDStr datatypeType;
    int bitLength;

    IODD_DT_RECORD_ITEM_T()
    {
        subIndex = 0;
        bitOffset = 0;
        bitLength = 0;
    }

    void PrintDebug()
    {
        IODD_DEBUG("---->RecordItem");
        IODD_DEBUG("IODD_DT_RECORD_ITEM_T::subIndex:       %d", subIndex);
        IODD_DEBUG("IODD_DT_RECORD_ITEM_T::bitOffset:      %d", bitOffset);
        IODD_DEBUG("IODD_DT_RECORD_ITEM_T::datatypeType:   %s", datatypeType.c_str());
        IODD_DEBUG("IODD_DT_RECORD_ITEM_T::bitLength:      %d", bitLength);
        IODD_DEBUG("////////////////////////////////////////////////////");
    }
};
typedef std::vector<IODD_DT_RECORD_ITEM_T> DataTypeRecordItemVect;

struct IODD_DATA_TYPE_T {
    IODDStr      id;
    IODDStr      type;
    int         bitLength;
    IODDStrVect  subTextIds;

    DataTypeRecordItemVect recordItems;

    void PrintDebug() {
        IODD_DEBUG("\tDatatype_T");
        IODD_DEBUG("\t\tID:             %s", id.c_str());
        IODD_DEBUG("\t\ttype:           %s", type.c_str());
        IODD_DEBUG("\t\tbitLength:      %d", bitLength);
    }
};
typedef std::vector<IODD_DATA_TYPE_T>   DataTypeVect;

class IODD_DataTypeParser
{
private:
    /* data */
    ezxml_t _datatypeCollectionNode;
public:
    IODD_DataTypeParser(ezxml_t datatypeCollection){
        _datatypeCollectionNode = datatypeCollection;
    };
    ~IODD_DataTypeParser(){};

    DataTypeVect Parse();
};

#endif