#ifndef IODD_MENU_H
#define IODD_MENU_H

#include "ezxml.h"
#include "types.h"

/*
<Menu id="M_OR_Calibration_1002">
<Name textId="TI_M_Calibration_Name" />
<VariableRef variableId="V_COF" displayFormat="Dec" gradient="0.18" offset="0" unitCode="1002" accessRightRestriction="ro" />
</Menu>
<Menu id="M_OR_Observation_1002">
<Name textId="TI_PdIn_Name" />
<RecordItemRef variableId="V_ProcessDataIn" subindex="1" gradient="0.18" offset="32" displayFormat="Dec" unitCode="1002" accessRightRestriction="ro" />
</Menu>
*/
struct IODD_MENU_RECORD_T {
    IODDStr  variableId;
    int     subIndex;
    float    gradient;
    int     offset;
    IODDStr  displayFormat;
    int     unitCode;
    IODDStr  accessRightRestriction;

    IODD_MENU_RECORD_T() {
        subIndex        = 0;
        gradient        = 1.0;
        offset          = 0;
        unitCode        = 0;
    }
    void PrintDebug() {
        IODD_DEBUG("\t\tMenu Variable");
        IODD_DEBUG("\t\t\tvariableId:                %s", variableId.c_str());
        IODD_DEBUG("\t\t\tsubIndex:                  %d", subIndex);
        IODD_DEBUG("\t\t\tgradient:                  %f", gradient);
        IODD_DEBUG("\t\t\toffset:                    %d", offset);
        IODD_DEBUG("\t\t\tdisplayFormat:             %s", displayFormat.c_str());
        IODD_DEBUG("\t\t\tunitCode:                  %d", unitCode);
        IODD_DEBUG("\t\t\taccessRightRestriction:    %s", accessRightRestriction.c_str());
    }
};
typedef std::vector<IODD_MENU_RECORD_T>   IODD_MenuRecordVect;

struct IODD_MENU_T {
/*
menu    -> menuRef -> condition -> variableId, value
        -> name, variableRef    -> variableId, displayFormat, gradient, offset, unitCode
menuRef -> name, recordItemRef  -> variableId, displayFormat, gradient, offset, unitCode, subindex
*/

    IODDStr            menuId;
    IODDStr            nameId;
    IODD_MenuRecordVect menuRecords;
    IODDStrPair        condition; // Variable ID and value

    IODD_MENU_T() {
        menuId      = "";
        nameId      = "";
    }

    void PrintDebug() {
        IODD_DEBUG("\tmenuId:               %s", menuId.c_str());
        IODD_DEBUG("\tnameId:               %s", nameId.c_str());
        IODD_DEBUG("\tCondition Variable ID:%s", condition.first.c_str());
        IODD_DEBUG("\tCondition Value:      %s", condition.second.c_str());
        IODD_DEBUG("\tMenuItems: %zu", menuRecords.size());
        for( auto &it : menuRecords) {
            it.PrintDebug();
        }
        IODD_DEBUG("---------------------------------------------------");
    }
};
typedef std::vector<IODD_MENU_T>   MenuVect;

class IODD_MenuParser
{
private:
    /* data */
    ezxml_t _userInterface;
public:
    IODD_MenuParser(ezxml_t userInterface){
        _userInterface = userInterface;
    };
    ~IODD_MenuParser(){};

    IODD_MenuRecordVect ParseMenuItem(ezxml_t menucollection, IODDStr menuId, IODDStr &nameId);
    MenuVect Parse();
};
#endif