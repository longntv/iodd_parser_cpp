#include <iostream>
#include <string>
#include "iodd_parser.h"
#include "iodd_helper.h"

void IODD_Parser::Parse() {
    // TAG route: 
    //    IODevice > ProfileBody > DeviceFunction > DatatypeCollection > Datatype 
    //                                           > VariableCollection > Variable > Condition
    //                                           > ProcessDataCollection > ProcessData > ProcessDataIn > Datatype > RecordItem
    //                                           > UserInterface > ObserverRoleMenuSet > ObservationMenu > MenuCollection > Menu > RecordItemRef
    //                                                           > MenuCollection > Menu > MenuRef > RecordItemRef
    _ioddData->deviceIdentity                = _deviceIdentityParser->Parse();
    _ioddData->dataTypeCollection            = _dataTypeParser->Parse();
    _ioddData->variableCollection            = _variableParser->Parse();
    _ioddData->menuCollection                = _menuParser->Parse();    
    _ioddData->processDataInCollection       = _processDataParser->Parse();

    // filter condition variable list
    for (auto &processData : _ioddData->processDataInCollection) {
        for (auto &condition : processData.conditions) {
            auto found = find_if(_ioddData->variableCollection.begin(),
                                 _ioddData->variableCollection.end(),
                                 [&condition](IODD_VARIABLE_T const& v) {
                                    return  condition.first == v.id;
                                });
            if(found != _ioddData->variableCollection.end()) {
                _ioddData->conditionVariableMap[condition.first] = std::make_shared<IODD_VARIABLE_T>(*found);
            }
        }
    }

    // filter condition menu list
    for (auto &menu : _ioddData->menuCollection) {
        if(!menu.condition.first.empty()) {
            auto found = find_if(_ioddData->variableCollection.begin(),
                                 _ioddData->variableCollection.end(),
                                 [&menu](IODD_VARIABLE_T const& v) {
                                    return  menu.condition.first == v.id;
                                });
            if(found != _ioddData->variableCollection.end()) {
                _ioddData->conditionVariableMap[menu.condition.first] = std::make_shared<IODD_VARIABLE_T>(*found);
            }
        }
    }
    _ioddData->PrintDebug();
}
