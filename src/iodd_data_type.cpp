#include "iodd_data_type.h"
#include "iodd_helper.h"

DataTypeVect IODD_DataTypeParser::Parse() {
    DataTypeVect dataTypeVect;
    for (ezxml_t datatypeNode = ezxml_child(_datatypeCollectionNode, "Datatype"); datatypeNode; datatypeNode = datatypeNode->next) {
        IODD_DATA_TYPE_T dataTypeTmp;
        XMLGetNumeric(datatypeNode, "bitLength",    dataTypeTmp.bitLength, T_INT);
        XMLGetStr(datatypeNode,     "id",           dataTypeTmp.id);
        XMLGetStr(datatypeNode,     "xsi:type",     dataTypeTmp.type);
        for (ezxml_t singleValueNode = ezxml_child(datatypeNode, "SingleValue"); singleValueNode; singleValueNode = singleValueNode->next) {
            IODDStr subTextId;
            ezxml_t singleValueNameNode = ezxml_child(singleValueNode, "Name");
            if(singleValueNameNode) {
                XMLGetStr(singleValueNameNode, "textId", subTextId);
                dataTypeTmp.subTextIds.push_back(subTextId);
            }
        }
        for (ezxml_t recordItemNode = ezxml_child(datatypeNode, "RecordItem"); recordItemNode; recordItemNode = recordItemNode->next) {
            IODD_DT_RECORD_ITEM_T recordItemTmp;
            XMLGetNumeric(recordItemNode, "subindex", recordItemTmp.subIndex, T_INT);
            XMLGetNumeric(recordItemNode, "bitOffset", recordItemTmp.bitOffset, T_INT);
            auto simpleDatatypeNode = ezxml_child(recordItemNode, "SimpleDatatype");
            if (simpleDatatypeNode) {
                XMLGetStr(simpleDatatypeNode, "xsi:type", recordItemTmp.datatypeType);
                XMLGetNumeric(simpleDatatypeNode, "bitLength", recordItemTmp.bitLength, T_INT);
            }
            dataTypeTmp.recordItems.push_back(recordItemTmp);
        }

        dataTypeVect.push_back(dataTypeTmp);
    }
    return dataTypeVect;
}