#ifndef ADVANTECH_PCI_1724U_H
#define ADVANTECH_PCI_1724U_H

/**********************************************
  Advantech_PCI_1724U.h (requires Advantech_PCI_1724U.cpp) defines a class that can communicate
    with the Advantech PCI-1724U analog output card using the comedilib drivers.  It will find the
    card in the /dev/comedi* list and then connect to it.  This class only supports one card currently.

  The Advantech PCI-1724U analog output card prives 32 +/- 10V outputs.

  Functions:
    writeVoltage(double volts, unsigned int channel) -- sends a command to the card to output the desired
        voltage on the desired channel.  Returns true if sucessful.

    double readCurrentVoltage( unsigned int channel ) -- reports from the card the current voltage being commanded.

    unsigned int getNumChannels() -- returns the maximum number of channels for the card (32).

**********************************************/

#include <comedilib.h>
#include <string>

#define NUM_CHANNELS 32

struct linearCal {
    double multiplier;
    double shift;
};

class Advantech_PCI_1724U
{

    std::string comediFileName;
    unsigned int comediDeviceMaxChannels;
    unsigned int comediSubDeviceNumber;
    static const unsigned int comediDeviceRangeNumber = 0; // this is the +/- 10 V output range
    comedi_range * comediDeviceRangeInfo[NUM_CHANNELS];
    linearCal calibration[NUM_CHANNELS];
    lsampl_t comediDeviceMaxData;
    comedi_t *comediDevice;
    bool calibrationMode;

public:
    Advantech_PCI_1724U();
    ~Advantech_PCI_1724U();
    bool writeVoltage(double volts, unsigned int channel);
    double readCurrentVoltage( unsigned int channel );
    unsigned int getNumChannels() const;
    void setCalibrationMode( bool calibrationModeOn );
};

#endif // ADVANTECH_PCI_1724U_H
