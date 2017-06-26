#include "Advantech_PCI_1724U.h"

#include <iostream>
#include <sstream>
#include <assert.h>
#include <comedilib.h>
#include <cmath>

using namespace std;

Advantech_PCI_1724U::Advantech_PCI_1724U():
    calibrationMode(false)
{
    std::stringstream comediFileName_strstream;
    for( int i=0; i<10; i++)
    {
        comediFileName_strstream.flush();
        comediFileName_strstream << "/dev/comedi" << i;
        comediFileName = comediFileName_strstream.str();
        comediDevice = comedi_open(comediFileName.c_str() );

        // Check to see if file was sucessfully opened
        if(comediDevice == NULL)
        {
            // if not skip to the next
            continue;
        }

        // Check to see if it is an advantech PCI-1724U board
        string boardName = comedi_get_board_name(comediDevice);
        if( 0 == boardName.compare("adv_pci1724") )
        {
            // If it is break out of the loop, were done :)
            break;
        } else {
            // If not set the device pointer back to NULL, this is not our board.
            comediDevice = NULL;
            comediFileName = "";
        }

    }
    // Check to see if file was sucessfully opened
    if( comediDevice == NULL )
    {
        cout << "ERROR: No Advantech PCI-1724 board found.  Please verify." << endl;
        assert( comediDevice != NULL );
    }

    cout << "Advantech PCI-1724U Sucessfully Opened on " << comediFileName << "... Initializing Parameters... ";


    //cout << "Finding Analog Output Sub Device Number" << endl;

    comediSubDeviceNumber = comedi_find_subdevice_by_type(comediDevice, COMEDI_SUBD_AO, 0);
    //cout << "comedi subdevice is number: " << comediSubDeviceNumber << endl;

    comediDeviceMaxChannels = comedi_get_n_channels(comediDevice, comediSubDeviceNumber);
    cout << "Max Channels: " << comediDeviceMaxChannels << endl;

    //cout << "Find Comedi range info" << endl;
    for(int i=0; i< NUM_CHANNELS; i++ )
    {
        comediDeviceRangeInfo[i] = comedi_get_range(comediDevice, comediSubDeviceNumber, i, comediDeviceRangeNumber);
        comediDeviceRangeInfo[i] ->max = 10;
        comediDeviceRangeInfo[i] ->min = -10;
    }



    struct rangeInfo {
        double min;
        double max;
    };

    calibration[0].multiplier = .9720;
    calibration[0].shift = .0386;
    calibration[1].multiplier = .976150163635117;
    calibration[1].shift = .040998306872675;
    calibration[2].multiplier = .9763;
    calibration[2].shift = .0395;
    calibration[3].multiplier = .9761;
    calibration[3].shift = .0416;
    calibration[4].multiplier = .9755;
    calibration[4].shift = .0402;

    calibration[5].multiplier = .9762;
    calibration[5].shift = .0410;
    calibration[6].multiplier = .9764;
    calibration[6].shift = .0370;
    calibration[7].multiplier = .9768;
    calibration[7].shift = .0389;
    calibration[8].multiplier = .9763;
    calibration[8].shift = .0430;
    calibration[9].multiplier = .9754;
    calibration[9].shift = .0480;

    calibration[10].multiplier = .9763;
    calibration[10].shift = .0461;
    calibration[11].multiplier = .9765;
    calibration[11].shift = .0452;
    calibration[12].multiplier = .9762;
    calibration[12].shift = .0415;
    calibration[13].multiplier = .9762;
    calibration[13].shift = .0455;
    calibration[14].multiplier = .9763;
    calibration[14].shift = .0494;

    calibration[15].multiplier = .9759;
    calibration[15].shift = .0437;
    calibration[16].multiplier = 0; // PROBLEM PIN!
    calibration[16].shift = 0; // PROBLEM PIN!
    calibration[17].multiplier = .9753;
    calibration[17].shift = .0334;
    calibration[18].multiplier = .9761;
    calibration[18].shift = .0343;
    calibration[19].multiplier = .9754;
    calibration[19].shift = .0331;
    calibration[20].multiplier = .9766;
    calibration[20].shift = .0313;
    calibration[21].multiplier = .9762;
    calibration[21].shift = .0362;
    calibration[22].multiplier = .9757;
    calibration[22].shift = .0348;
    calibration[23].multiplier = .9769;
    calibration[23].shift = .0355;
    calibration[24].multiplier = .9762;
    calibration[24].shift = .0628;
    calibration[25].multiplier = .9758;
    calibration[25].shift = .0674;
    calibration[26].multiplier = .9756;
    calibration[26].shift = .0647;
    calibration[27].multiplier = .9770;
    calibration[27].shift = .0664;
    calibration[28].multiplier = .9765;
    calibration[28].shift = .0650;
    calibration[29].multiplier = .9761;
    calibration[29].shift = .0645;
    calibration[30].multiplier = .975860709072996;
    calibration[30].shift = .059438788643537;
    calibration[31].multiplier = 0.0; // PROBLEM PIN
    calibration[31].shift = 0.0; // PROBLEM PIN

    rangeInfo RangeInfo[NUM_CHANNELS];
    //cout << "Get Max Data" << endl;
    comediDeviceMaxData = comedi_get_maxdata(comediDevice, comediSubDeviceNumber, comediDeviceRangeNumber);

    //cout << "Max Data is: " << comediDeviceMaxData << endl;

    //cout << "Get Max Number of Channels: ";


    //cout << "Done" << endl;

}

Advantech_PCI_1724U::~Advantech_PCI_1724U()
{
    for (unsigned int channelNumber = 0; channelNumber<comediDeviceMaxChannels; channelNumber++)
    {
        if( channelNumber != 16 && channelNumber != 31 )
        {
            writeVoltage(0, channelNumber);
        }
    }
    comedi_close(comediDevice);
    cout << "Advantech PCI-1724U Sucessfully Closed" << endl;

}

bool Advantech_PCI_1724U::writeVoltage(double volts, unsigned int channel)
{
    if( channel == 16 || channel == 31 )
    {
        cout << "WARNING! Advantech PCI 1724U Currently does not send proper voltages out on A16 and A31.  You requested a voltage of: "
             << volts << " on channel A" << channel << " Which will not be sucessful." << "\n\terror message in: Advantech_PCI_1724U.cpp line 170" << endl;

        assert( channel != 16 && channel != 31 );
        return false;
    }
    // Convert voltage into the unsigned version used by the comedi worlds

    if( !calibrationMode )
        volts = calibration[0].multiplier * volts + calibration[0].shift;

    lsampl_t comediVolts = comedi_from_phys(volts, comediDeviceRangeInfo[channel], comediDeviceMaxData);


    // Command voltage and print error if there is one
    int error = comedi_data_write(comediDevice, comediSubDeviceNumber, channel, comediDeviceRangeNumber, AREF_GROUND, comediVolts);

    if( error == -1 )
    {
        comedi_perror(comediFileName.c_str());
        return false;
    }

    return true;
}

unsigned int Advantech_PCI_1724U::getNumChannels() const
{
    return comediDeviceMaxChannels;
}

double Advantech_PCI_1724U::readCurrentVoltage( unsigned int channel )
{
    // verify requrested channel is valid
    assert( channel < comediDeviceMaxChannels );


    lsampl_t comediVolts;

    int error = comedi_data_read(comediDevice, comediSubDeviceNumber, channel, comediDeviceRangeNumber, AREF_GROUND, &comediVolts);
    if( error == -1 )
    {
        comedi_perror(comediFileName.c_str());
        return false;
    }

    // Comedi to phys is NAN if at the top or bottom of the scale
    if ( comediVolts == 0 )
        return comediDeviceRangeInfo[channel]->min;
    else if( comediVolts == comediDeviceMaxData )
        return comediDeviceRangeInfo[channel]->max;
    else
        return comedi_to_phys(comediVolts, comediDeviceRangeInfo[channel], comediDeviceMaxData);

}


void Advantech_PCI_1724U::setCalibrationMode( bool calibrationModeOn )
{
    calibrationMode = calibrationModeOn;
}
