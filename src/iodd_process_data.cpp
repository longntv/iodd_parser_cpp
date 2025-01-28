#include "iodd_process_data.h"
#include "iodd_helper.h"
#include "iodd_data_type.h"

IODD_PROCESS_DATA_REF_T * IODD_ProcessDataParser::FindProcessDataRefById(IODDStr id) {
    if (_processDataRefCollection.empty()) {
        return NULL;
    }
    for(auto &it : _processDataRefCollection) {
        if (it.processDataInId == id) {
            return &it;
        }
    }
    return NULL;
}

// Function to parse RecordItems
ProcessDataRecordItemVect IODD_ProcessDataParser::ParseRecordItems(IODDStr processDataInId, ezxml_t datatypeNode) {
    ProcessDataRecordItemVect recordItems;
    for (ezxml_t recordItem = ezxml_child(datatypeNode, "RecordItem"); recordItem; recordItem = recordItem->next) {
        IODD_PD_RECORD_ITEM_T recordItemTmp;
        XMLGetNumeric(recordItem,   "subindex",     recordItemTmp.subindex,  T_INT);
        XMLGetNumeric(recordItem,   "bitOffset",    recordItemTmp.bitOffset, T_INT);

        auto nameTextIdNode = ezxml_child(recordItem, "Name");
        if (nameTextIdNode) {
            XMLGetStr(nameTextIdNode, "textId", recordItemTmp.nameTextId);
        }

        auto simpleDatatypeNode = ezxml_child(recordItem, "SimpleDatatype");
        if (simpleDatatypeNode) {
            XMLGetStr(simpleDatatypeNode,      "xsi:type",     recordItemTmp.type);
            XMLGetNumeric(simpleDatatypeNode,  "bitLength",    recordItemTmp.bitLength, T_INT);
            
            auto valueRangeTag = ezxml_child(simpleDatatypeNode,   "ValueRange");
            if (valueRangeTag) {
                XMLGetStr(valueRangeTag,   "lowerValue",   recordItemTmp.lowerValue);
                XMLGetStr(valueRangeTag,   "upperValue",   recordItemTmp.upperValue);
            }
        }
    
        auto datatypeRefNode = ezxml_child(recordItem, "DatatypeRef");
        if (datatypeRefNode) {
            IODDStr datatypeId;
            XMLGetStr(datatypeRefNode,      "datatypeId",   datatypeId);

            // find datatype by id to get type and bitLength
            // IODD_DATA_TYPE_T* datatype = FindDataTypeByID(datatypeId);
            IODD_DATA_TYPE_T* datatype = 
                FindById<IODD_DATA_TYPE_T>(*_dataTypeCollection, datatypeId, [](const IODD_DATA_TYPE_T& dataType) {
                    return dataType.id;
                });

            if (datatype) {
                recordItemTmp.type = datatype->type;
                recordItemTmp.bitLength = datatype->bitLength;
            }
        }
        // find process data ref to get display format, gradient, offset, unit code
        IODD_PROCESS_DATA_REF_T* processDataRef = FindProcessDataRefById(processDataInId);
        if (processDataRef) {
            for(auto &pdRefItem : processDataRef->pdRefItems) {
                if(pdRefItem.subindex == recordItemTmp.subindex) {
                    if(!pdRefItem.unitCode.empty()) {
                        recordItemTmp.unitCode      = pdRefItem.unitCode;
                    }
                    if(!pdRefItem.displayFormat.empty()) {
                        recordItemTmp.displayFormat = pdRefItem.displayFormat;
                    }
                    if(pdRefItem.gradient != 0) {
                        recordItemTmp.gradient = pdRefItem.gradient;
                    }
                    if(pdRefItem.offset != 0) {
                        recordItemTmp.offset = pdRefItem.offset;
                    }
                }

                // set default display format if not found
                if(recordItemTmp.displayFormat.empty()) {
                    if(recordItemTmp.type == RECORD_TYPE_BOOL || recordItemTmp.type == RECORD_TYPE_UINT || recordItemTmp.type == RECORD_TYPE_INT) {
                        recordItemTmp.displayFormat = "Dec";
                    }
                    if (recordItemTmp.type == RECORD_TYPE_FLOAT) {
                        recordItemTmp.displayFormat = "Dec.6";
                    }
                }
            }
        }
        recordItems.push_back(recordItemTmp);
    }
    return recordItems;
}

IODD_PROCESS_DATA_IO_T IODD_ProcessDataParser::ParseProcessDataInOut(ezxml_t processDataInNode)
{
    IODD_PROCESS_DATA_IO_T processDataIn;
    XMLGetStr(processDataInNode,        "id",           processDataIn.id);
    XMLGetNumeric(processDataInNode,    "bitLength",    processDataIn.bitLength, T_INT);

    ezxml_t datatypeNode = ezxml_child(processDataInNode, "Datatype");
    if (datatypeNode) {
        XMLGetStr(datatypeNode,         "type",         processDataIn.datatype.type);
        XMLGetNumeric(datatypeNode,     "bitLength",    processDataIn.datatype.bitLength, T_INT);
        processDataIn.datatype.recordItems = ParseRecordItems(processDataIn.id, datatypeNode);
    }
    ezxml_t nameTextIdNode = ezxml_child(processDataInNode, "Name");
    if (nameTextIdNode) {
        XMLGetStr(nameTextIdNode, "textId", processDataIn.nameTextId);
    }
    return processDataIn;
}

// Function to parse ProcessDataRef
void IODD_ProcessDataParser::ParseProcessDataRefCollection()
{
    for (ezxml_t processDataRefNode = ezxml_child(_processDataRefCollectionNode, "ProcessDataRef"); 
                 processDataRefNode; processDataRefNode = processDataRefNode->next) {
        IODD_PROCESS_DATA_REF_T processDataRef;
        XMLGetStr(processDataRefNode, "processDataId", processDataRef.processDataInId);

        for(ezxml_t processDataRefInfo = ezxml_child(processDataRefNode, "ProcessDataRecordItemInfo"); 
                    processDataRefInfo; processDataRefInfo = processDataRefInfo->next) {
            IODD_PROCESS_DATA_REF_ITEM_T processDataRefItem;
            
            XMLGetNumeric(processDataRefInfo,   "subindex", processDataRefItem.subindex, T_INT);
            XMLGetStr(processDataRefInfo,       "unitCode", processDataRefItem.unitCode);
            XMLGetStr(processDataRefInfo,       "displayFormat", processDataRefItem.displayFormat);
            XMLGetNumeric(processDataRefInfo,   "offset", processDataRefItem.offset, T_INT);
            XMLGetNumeric(processDataRefInfo,   "gradient", processDataRefItem.gradient, T_FLOAT);
            processDataRef.pdRefItems.push_back(processDataRefItem);
        }
        _processDataRefCollection.push_back(processDataRef);
    }
}

// Function to parse ProcessData
ProcessDataInVect IODD_ProcessDataParser::Parse()
{
    // parse ProcessDataRef first
    ParseProcessDataRefCollection();

    // parse ProcessData
    ProcessDataInVect processDataVect;
    for (ezxml_t processDataNode = ezxml_child(_processDataCollectionNode, "ProcessData"); 
                 processDataNode; processDataNode = processDataNode->next) {
        IODD_PROCESS_DATA_T processData;
        XMLGetStr(processDataNode, "id", processData.id);
        ezxml_t conditionNode = ezxml_child(processDataNode, "Condition");
        for (ezxml_t condition = conditionNode; condition; condition = condition->next) {
            IODDStr variableId;
            int value;
            XMLGetStr(condition,     "variableId",  variableId);
            XMLGetNumeric(condition, "value",       value, T_INT);
            processData.conditions.push_back(std::make_pair(variableId, value));
        }

        ezxml_t processDataInNode = ezxml_child(processDataNode, "ProcessDataIn");
        processData.processDataIn = ParseProcessDataInOut(processDataInNode);
        processDataVect.push_back(processData);
    }
    return processDataVect;
}
