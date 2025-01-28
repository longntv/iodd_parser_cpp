#include "iodd_variable.h"
#include "iodd_helper.h"

// Function to parse RecordItems
VarRecordItemVect IODD_VariableParser::ParseRecordItems(ezxml_t datatypeNode) {
    VarRecordItemVect recordItems;
    for (ezxml_t recordItemNode = ezxml_child(datatypeNode, "RecordItem"); recordItemNode; recordItemNode = recordItemNode->next) {
        IODD_VAR_RECORD_ITEM_T recordItem;

        XMLGetNumeric(recordItemNode,       "subindex",     recordItem.subIndex,    T_INT);
        XMLGetNumeric(recordItemNode,       "bitOffset",    recordItem.bitOffset,   T_INT);
        XMLGetStr(recordItemNode,           "defaultValue", recordItem.defaultValue);

        ezxml_t simpleDatatypeNode = ezxml_child(recordItemNode, "SimpleDatatype");
        if (simpleDatatypeNode) {
            XMLGetStr(simpleDatatypeNode,      "xsi:type",  recordItem.datatypeType);
            XMLGetNumeric(simpleDatatypeNode,  "bitLength", recordItem.bitLength,   T_INT);
            auto valueRangeNode = ezxml_child(simpleDatatypeNode, "ValueRange");
            if (valueRangeNode) {
                XMLGetNumeric(valueRangeNode, "lowerValue", recordItem.lowerValue, T_INT);
                XMLGetNumeric(valueRangeNode, "upperValue", recordItem.upperValue, T_INT);
            }
        }
        auto nameNode = ezxml_child(recordItemNode, "Name");
        XMLGetStr(nameNode,      "textId",      recordItem.nameTextId);
        recordItems.push_back(recordItem);
    }

    return recordItems;
}

// Function to parse SingleValues
SingleValueVect IODD_VariableParser::ParseSingleValues(ezxml_t datatypeNode) {
    SingleValueVect singleValues;
    for (ezxml_t singleValueNode = ezxml_child(datatypeNode, "SingleValue"); singleValueNode; singleValueNode = singleValueNode->next) {
        SingleValue singleValue;
        XMLGetStr(singleValueNode, "value", singleValue.value);
        auto nameNode = ezxml_child(singleValueNode, "Name");
        if(nameNode) XMLGetStr(nameNode, "textId", singleValue.textId);
        singleValues.push_back(singleValue);
    }

    return singleValues;
}

// Function to parse Datatype
IODD_VAR_DATA_TYPE_T IODD_VariableParser::ParseDatatype(ezxml_t datatypeNode) {
    IODD_VAR_DATA_TYPE_T datatype;
    XMLGetStr(datatypeNode,      "xsi:type",     datatype.type);
    XMLGetNumeric(datatypeNode,  "bitLength",    datatype.bitLength, T_INT);
    datatype.recordItems = ParseRecordItems(datatypeNode);
    datatype.singleValues = ParseSingleValues(datatypeNode);

    return datatype;
}

IODD_VAR_DATA_TYPE_T IODD_VariableParser::ParseDatatypeRef(ezxml_t datatypeRefNode) {
    IODD_VAR_DATA_TYPE_T datatype;
    IODDStr datatypeId;
    XMLGetStr(datatypeRefNode,      "datatypeId",   datatypeId);

    // find datatype by id to get type and bitLength
    // IODD_DATA_TYPE_T* datatype = FindDataTypeByID(datatypeId);
    IODD_DATA_TYPE_T* found = 
        FindById<IODD_DATA_TYPE_T>(*_dataTypeCollection, datatypeId, [](const IODD_DATA_TYPE_T& dataType) {
            return dataType.id;
        });

    if (found) {
        datatype.type = found->type;
        datatype.bitLength = found->bitLength;
        // iterate over record items
        for (auto r : found->recordItems) {
            IODD_VAR_RECORD_ITEM_T recordItem;
            recordItem.subIndex = r.subIndex;
            recordItem.bitOffset = r.bitOffset;
            recordItem.datatypeType = r.datatypeType;
            recordItem.bitLength = r.bitLength;

            datatype.recordItems.push_back(recordItem);
        }
    }

    return datatype;
}



// Function to parse Variables
VariableVect IODD_VariableParser::Parse() {
    VariableVect variables;

    for (ezxml_t variableNode = ezxml_child(_variableCollectionNode, "Variable"); variableNode; variableNode = variableNode->next) {
        IODD_VARIABLE_T variable;
        XMLGetStr(variableNode,         "id",               variable.id);
        XMLGetNumeric(variableNode,     "index",            variable.index, T_INT);
        XMLGetStr(variableNode,         "accessRights",     variable.accessRights);
        XMLGetStr(variableNode,         "defaultValue",        variable.defaultValue);

        auto nameNode = ezxml_child(variableNode, "Name");
        if (nameNode) {
            XMLGetStr(nameNode,         "textId",               variable.nameTextId);
        }
        auto datatypeNode = ezxml_child(variableNode, "Datatype");
        if (datatypeNode) {
            variable.datatype = ParseDatatype(datatypeNode);
        }

        auto datatypeRefNode = ezxml_child(variableNode, "DatatypeRef");
        if (datatypeRefNode) {
            variable.datatype = ParseDatatypeRef(datatypeRefNode);
        }

        auto recordItemInfoNode = ezxml_child(variableNode, "RecordItemInfo");
        for (auto recordItemInfoNode = ezxml_child(variableNode, "RecordItemInfo"); recordItemInfoNode; recordItemInfoNode = recordItemInfoNode->next) {
            // check record items exist in datatype and same subindex will update the datatype
            bool isRecordItemExist = false;
            IODD_VAR_RECORD_ITEM_T recordItem;
            XMLGetNumeric(recordItemInfoNode, "subindex", recordItem.subIndex, T_INT);
            for (auto &rit : variable.datatype.recordItems) {
                if (rit.subIndex == recordItem.subIndex) {
                    XMLGetStr(recordItemInfoNode, "defaultValue", rit.defaultValue);
                    isRecordItemExist = true;
                    break;
                }
            }
            if (!isRecordItemExist) {
                XMLGetStr(recordItemInfoNode, "defaultValue", recordItem.defaultValue);
                variable.datatype.recordItems.push_back(recordItem);
            }
        }

        variables.push_back(variable);
    }

    return variables;
}