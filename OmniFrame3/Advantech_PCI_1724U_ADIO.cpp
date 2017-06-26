#include "Advantech_PCI_1724U_ADIO.h"

Advantech_PCI_1724U_ADIO::Advantech_PCI_1724U_ADIO(unsigned int designator, ostream* outputStreamPointer):
    voltageOutputDevice(-10.001,10.001,32,designator,"Advantech PCI-1724U", outputStreamPointer),
    Advantech_PCI_1724U()
{
    return;
}


bool Advantech_PCI_1724U_ADIO::writeVoltages(const vector<writeValue>& channelsAndValues) {

    bool sucessful = true;


    for(unsigned int i=0; i< channelsAndValues.size();i++)
    {
        sucessful &= Advantech_PCI_1724U::writeVoltage(channelsAndValues[i].value, channelsAndValues[i].channelNumber);
    }


    return sucessful;
}

void Advantech_PCI_1724U_ADIO::setCalibrationMode( bool calibrationModeOn )
{

    Advantech_PCI_1724U::setCalibrationMode( calibrationModeOn );


}

