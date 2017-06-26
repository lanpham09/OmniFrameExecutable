#ifndef MULTIPLEOMNIMAGNETSETUP_H
#define MULTIPLEOMNIMAGNETSETUP_H

/*******************************************
  This Class handels the setup of the 7 Omnimagnets
  to make life easier in making other systems.

  The wiring is color coded thus:
  Green:  Inner
  Blue:   Middle
  Orange: Outer

  In the omnimagnet frame:
   Inner  Coils -> X direction Moment
   Middle Coils -> Y direction Moment
   Outer  Coils -> Z direction Moment

  omnimagnet0: (The big one)
    Inner  Current: PCI_1724 A1,  Break Out Board: 3(ground), 4(signal)
    Middle Current: PCI_1724 A9,  Break Out Board: 24(ground), 25(signal)
    Outer  Current: PCI_1724 A17, Break Out Board: 45(ground), 46(signal)
    Inhibit:        PCI_1724 A26, Break Out Board  41(ground), 19(signal)

  omnimagnet1:
    Inner  Current: PCI_1724 A2,  Break Out Board:  5(ground),  6(signal)
    Middle Current: PCI_1724 A10, Break Out Board: 26(ground), 27(signal)
    Outer  Current: PCI_1724 A18, Break Out Board: 47(ground), 48(signal)
    Inhibit:        PCI_1724 A26, Break Out Board  41(ground), 19(signal)

  omnimagnet2:
    Inner  Current: PCI_1724 A3,  Break Out Board:  7(ground),  8(signal)
    Middle Current: PCI_1724 A11, Break Out Board: 28(ground), 29(signal)
    Outer  Current: PCI_1724 A19, Break Out Board: 49(ground), 50(signal)
    Inhibit:        PCI_1724 A26, Break Out Board  41(ground), 19(signal)

  omnimagnet3:
    Inner  Current: PCI_1724 A4,  Break Out Board:  9(ground), 10(signal)
    Middle Current: PCI_1724 A12, Break Out Board: 30(ground), 31(signal)
    Outer  Current: PCI_1724 A20, Break Out Board: 51(ground), 52(signal)
    Inhibit:        PCI_1724 A26, Break Out Board  41(ground), 19(signal)

  omnimagnet4:
    Inner  Current: PCI_1724 A5,  Break Out Board: 11(ground), 12(signal)
    Middle Current: PCI_1724 A13, Break Out Board: 32(ground), 33(signal)
    Outer  Current: PCI_1724 A21, Break Out Board: 53(ground), 54(signal)
    Inhibit:        PCI_1724 A26, Break Out Board  41(ground), 19(signal)

  omnimagnet5:
    Inner  Current: PCI_1724 A6,  Break Out Board: 13(ground), 14(signal)
    Middle Current: PCI_1724 A14, Break Out Board: 34(ground), 35(signal)
    Outer  Current: PCI_1724 A22, Break Out Board: 55(ground), 56(signal)
    Inhibit:        PCI_1724 A26, Break Out Board  41(ground), 19(signal)

  omnimagnet6:
    Inner  Current: PCI_1724 A7,  Break Out Board: 15(ground), 16(signal)
    Middle Current: PCI_1724 A15, Break Out Board: 36(ground), 37(signal)
    Outer  Current: PCI_1724 A23, Break Out Board: 57(ground), 58(signal)
    Inhibit:        PCI_1724 A26, Break Out Board  41(ground), 19(signal)


    This class also provides public access to:
        The PCI_1724U voltage drive (voltageDrive)
        The AMC 30A8  current drives (currentDrive[])
        The file logger (dataLogger)
        The system inhibit (systemInhibit)
        The system auto inhibit (systemAutoInhibit)


    You will need to call changeConfiguration6DOF_Producer( otherProducer ); on each omnimagnet after this
        class is instantiated to set the position and orientation for your application.

    You will need to call bool setupTempMeasurement(Coil coil, analogInputDevice* measurementDevice, unsigned int device_channel); on each omnimagnet after this
        class is instantiated to set the device which will read temperatures of the coils.



*******************************************/

#include "Devices/Omnimagnet.h"
#include "Devices/Advantech_PCI_1724U_ADIO.h"
#include "Devices/AMC_30A8.h"
#include "AD_IO/vectorCurrentOutput.h"
#include "AD_IO/deviceInhibit.h"
#include "AD_IO/autoInhibit.h"
#include "Utilities/Units.h"
#include "Utilities/Math.h"
#include "Utilities/utilities.h"

#include <string>

enum omnimagnetNumber{ OmniMag0, OmniMag1, OmniMag2, OmniMag3, OmniMag4, OmniMag5, OmniMag6 };

class MultipleOmnimagnetSetup: public AD_IO::deviceInhibit
{
private:
    Omnimagnet* omnimagnet0; // The Big One

    Omnimagnet* omnimagnet1;
    Omnimagnet* omnimagnet2;
    Omnimagnet* omnimagnet3;
    Omnimagnet* omnimagnet4;
    Omnimagnet* omnimagnet5;
    Omnimagnet* omnimagnet6;

    static const int NUM_CURRENT_DRIVES = 21;
    static const int NUM_VECTOR_OUT_DEVICE = NUM_CURRENT_DRIVES/3;

    AD_IO::vectorCurrentOutput* vectorOutDevice[NUM_VECTOR_OUT_DEVICE];

public:
    MultipleOmnimagnetSetup(std::string logDirectoryName = "Omnimagnet System Data Log Files", bool enable_auto_inhibit_functionality = true);
    ~MultipleOmnimagnetSetup();

    Advantech_PCI_1724U_ADIO voltageDrive;

    //AD_IO::deviceInhibit    *systemInhibit;         // THIS LINE WAS COMMENTED OUT BY ANDREW.  WHO KNOWS WHY...
    AD_IO::autoInhibit      *systemAutoInhibit;

    AMC_30A8* currentDrive[NUM_CURRENT_DRIVES];

    AD_IO::fileLogger* dataLogger;

    Omnimagnet* operator()(omnimagnetNumber num);



};

#endif // MULTIPLEOMNIMAGNETSETUP_H
