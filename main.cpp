#include "iodd_sensor.h"

int main() {
    IODD_Sensor sensorAT00x, sensorBCM_R15E;
    sensorAT00x.LoadSensor("837", "6", "AT001", "ioddfiles/AT00x.xml");
    // proccess value
    sensorAT00x.ProcessValue("0105");

    sensorBCM_R15E.LoadSensor("310", "1367", "BCM_R15E", "ioddfiles/Balluff-BCM_R15E-002-DI00-20200327-IODD1.1.xml");
    return 0;
}