#include "iodd_menu.h"
#include "iodd_helper.h"

IODD_MenuRecordVect IODD_MenuParser::ParseMenuItem(ezxml_t menucollection, IODDStr menuId, IODDStr &nameId) {
    IODD_MenuRecordVect menuRecords;
    for (auto menuTag = ezxml_child(menucollection, "Menu"); menuTag; menuTag = menuTag->next) 
    {
        IODDStr menuWlk;
        XMLGetStr(menuTag, "id", menuWlk);
        if(menuWlk == menuId) {
            auto nameTag = ezxml_child(menuTag, "Name");
            XMLGetStr(nameTag, "textId", nameId);
            for (auto recordItemRefTag = ezxml_child(menuTag, "RecordItemRef"); recordItemRefTag; recordItemRefTag = recordItemRefTag->next)
            {
                IODD_MENU_RECORD_T menuItemTmp;
                XMLGetStr(recordItemRefTag,             "variableId",       menuItemTmp.variableId);
                XMLGetNumeric(recordItemRefTag,         "subindex",         menuItemTmp.subIndex, T_INT);
                XMLGetNumeric(recordItemRefTag,         "unitCode",         menuItemTmp.unitCode, T_INT);
                XMLGetNumeric(recordItemRefTag,         "offset",           menuItemTmp.offset,   T_INT);
                XMLGetNumeric(recordItemRefTag,         "gradient",         menuItemTmp.gradient, T_FLOAT);
                XMLGetStr(recordItemRefTag,             "displayFormat",    menuItemTmp.displayFormat);
                menuRecords.push_back(menuItemTmp);
            }
        }
    }
    return menuRecords;
}
 
/*
menu    -> menuRef -> condition -> variableId, value
        -> name, variableRef    -> variableId, displayFormat, gradient, offset, unitCode
menuRef -> name, recordItemRef  -> variableId, displayFormat, gradient, offset, unitCode, subindex
*/
MenuVect IODD_MenuParser::Parse() {
    auto menuCollection         = ezxml_child(_userInterface,           "MenuCollection");
    auto observerRoleMenuSetTag = ezxml_child(_userInterface,           "ObserverRoleMenuSet");
    auto observationMenuTag     = ezxml_child(observerRoleMenuSetTag,   "ObservationMenu");
    MenuVect menuVect;
    IODDStr observationMenuId, menuWlk;
    XMLGetStr(observationMenuTag, "menuId", observationMenuId);
    for (auto menuTag = ezxml_child(menuCollection, "Menu"); menuTag; menuTag = menuTag->next) {
        XMLGetStr(menuTag, "id", menuWlk);
        if(menuWlk == observationMenuId) {
            // check this menu has condition or not
            if(ezxml_child(menuTag, "MenuRef")) {
                for(auto menuRefTag = ezxml_child(menuTag, "MenuRef"); menuRefTag; menuRefTag = menuRefTag->next) {
                    IODD_MENU_T menuTmp;
                    XMLGetStr(menuRefTag, "menuId", menuTmp.menuId);
                    auto conditionTag = ezxml_child(menuRefTag, "Condition");
                    if(conditionTag) {
                        XMLGetStr(conditionTag, "variableId",   menuTmp.condition.first);
                        XMLGetStr(conditionTag, "value",        menuTmp.condition.second);
                    }
                    menuTmp.menuRecords = ParseMenuItem(menuCollection, menuTmp.menuId, menuTmp.nameId);
                    menuVect.push_back(menuTmp);
                }
            } else {
                IODD_MENU_T menuTmp;
                menuTmp.menuId = observationMenuId;
                auto nameTag = ezxml_child(menuTag, "Name");
                XMLGetStr(nameTag, "textId", menuTmp.nameId);
                for(auto recordItemRefTag = ezxml_child(menuTag, "RecordItemRef"); recordItemRefTag; recordItemRefTag = recordItemRefTag->next) {
                    IODD_MENU_RECORD_T menuItemTmp;
                    XMLGetNumeric(recordItemRefTag, "subindex", menuItemTmp.subIndex, T_INT);
                    XMLGetNumeric(recordItemRefTag, "unitCode", menuItemTmp.unitCode, T_INT);
                    XMLGetNumeric(recordItemRefTag, "offset",   menuItemTmp.offset,   T_INT);
                    XMLGetNumeric(recordItemRefTag, "gradient", menuItemTmp.gradient, T_FLOAT);
                    XMLGetStr(recordItemRefTag,     "displayFormat",  menuItemTmp.displayFormat);
                    XMLGetStr(recordItemRefTag,     "variableId",     menuItemTmp.variableId);
                    menuTmp.menuRecords.push_back(menuItemTmp);
                }
                for(auto variableRefTag = ezxml_child(menuTag, "VariableRef"); variableRefTag; variableRefTag = variableRefTag->next) {
                    IODD_MENU_RECORD_T menuItemTmp;
                    XMLGetStr(variableRefTag, "variableId", menuItemTmp.variableId);
                    XMLGetStr(variableRefTag, "displayFormat", menuItemTmp.displayFormat);
                    XMLGetNumeric(variableRefTag, "gradient", menuItemTmp.gradient, T_FLOAT);
                    XMLGetNumeric(variableRefTag, "offset",   menuItemTmp.offset,   T_INT);
                    XMLGetNumeric(variableRefTag, "unitCode", menuItemTmp.unitCode, T_INT);
                    menuTmp.menuRecords.push_back(menuItemTmp);
                }
                menuVect.push_back(menuTmp);
            }
        }
    }
    return menuVect;
}
