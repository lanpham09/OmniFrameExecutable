#ifndef ADVANTECH_PCI_1724U_ADIO_H
#define ADVANTECH_PCI_1724U_ADIO_H

/**********************************************
  Advantech_PCI_1724U_ADIO.h (requires Advantech_PCI_1724U_ADIO.cpp) defines an ADIO::voltageOutputDevice using
    the Advantech PCI-1724U analog output card.  This class inherits the Advantech_PCI_1724U class.

    all public functions inherited from ADIO::voltageOutputDevice

**********************************************/

#include "Devices/Advantech_PCI_1724U.h"
#include "AD_IO/AD_IO.h"
using namespace AD_IO;

namespace AD_IO {
class Advantech_PCI_1724U_ADIO: public voltageOutputDevice, protected Advantech_PCI_1724U
{

protected:
    virtual bool writeVoltages(const vector<writeValue>& channelsAndValues);

public:
    Advantech_PCI_1724U_ADIO(unsigned int designator = 0, ostream* outputStreamPointer = &cout);
    void setCalibrationMode( bool calibrationModeOn );




};

}// End Namespace AD_IO

#endif // ADVANTECH_PCI_1724U_ADIO_H
