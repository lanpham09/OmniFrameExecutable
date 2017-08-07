#include "AMC_30A8.h"

#include <string>
#include <ostream>
using namespace std;

using namespace AD_IO;
#define NUM_POINTS 500

bool	AMC_30A8::writeCurrents(const vector<writeValue>& channelsAndValues) {

    if( isEnabled() || channelsAndValues[0].value.value ==0 ) {



        __outputVoltage.p_signal->append( channelsAndValues[0].value * AMC_30A8_VOLTS_PER_AMP ); // add value to voltages list

        return __outputVoltage.p_device->write( __outputVoltage.deviceChannel ); // tell device to write value to redgister

    } else {

        *__p_outputStream << name() << " is Inhibited.  Cannot send current signal." << endl;
        __outputVoltage.p_signal->append( 0.0 ); // add value to voltages list
        return __outputVoltage.p_device->write( __outputVoltage.deviceChannel ); // tell device to write value to redgister

    }

}


AMC_30A8::AMC_30A8(_outputDevice* vOutDevice, unsigned int outputChannel, unsigned int inhibitChannel, deviceInhibit* p_systemInhibit, string name, ostream* p_outputStream ): currentOutputDevice( AMC_30A8_MAX_CURRENT, AMC_30A8_NUM_CHANNELS, 0, name, p_outputStream),  deviceInhibit( p_systemInhibit )
{
     AMC_Initializer(vOutDevice, outputChannel);
    /*** initialize the inhibit output information ***/
    __outputInhibit.deviceChannel	= inhibitChannel;
    __outputInhibit.p_device		= vOutDevice;
    __outputInhibit.p_signal		= deviceInhibit::inhibitSignalPointer();  // add inhibit signal to structure

    __outputInhibit.p_device->addSignal(__outputInhibit.deviceChannel,__outputInhibit.p_signal); // add it to the output device

    __outputInhibit.p_device->setBoolInterp_True(__outputInhibit.deviceChannel,INHIBIT_VOLTAGE); // Define desired inhibit voltage response
    __outputInhibit.p_device->setBoolInterp_False(__outputInhibit.deviceChannel,ENABLE_VOLTAGE); // Define desired inhibit voltage response

}
AMC_30A8::AMC_30A8(_outputDevice* vOutDevice, unsigned int outputChannel, unsigned int inhibitChannel, string name, ostream* p_outputStream ):
            currentOutputDevice( AMC_30A8_MAX_CURRENT, AMC_30A8_NUM_CHANNELS, 0, name, p_outputStream),  deviceInhibit( name + " Inhibit" )
{
     AMC_Initializer(vOutDevice, outputChannel);
    /*** initialize the inhibit output information ***/
    __outputInhibit.deviceChannel		= inhibitChannel;
    __outputInhibit.p_device		= vOutDevice;
    __outputInhibit.p_signal		= deviceInhibit::inhibitSignalPointer();  // add inhibit signal to structure

    __outputInhibit.p_device->addSignal(__outputInhibit.deviceChannel,__outputInhibit.p_signal); // add it to the output device

    __outputInhibit.p_device->setBoolInterp_True(__outputInhibit.deviceChannel,INHIBIT_VOLTAGE); // Define desired inhibit voltage response
    __outputInhibit.p_device->setBoolInterp_False(__outputInhibit.deviceChannel,ENABLE_VOLTAGE); // Define desired inhibit voltage response

}
AMC_30A8::AMC_30A8(_outputDevice* vOutDevice, unsigned int outputChannel, _outputDevice* inhibitOutDevice, unsigned int inhibitChannel, deviceInhibit* p_systemInhibit, string name, ostream* p_outputStream ):
            currentOutputDevice( AMC_30A8_MAX_CURRENT, AMC_30A8_NUM_CHANNELS, 0, name, p_outputStream),  deviceInhibit( p_systemInhibit )
{
    AMC_Initializer(vOutDevice, outputChannel);
    /*** initialize the inhibit output information ***/
    __outputInhibit.deviceChannel		= inhibitChannel;
    __outputInhibit.p_device		= inhibitOutDevice;
    __outputInhibit.p_signal		= deviceInhibit::inhibitSignalPointer();  // add inhibit signal to structure

    __outputInhibit.p_device->addSignal(__outputInhibit.deviceChannel,__outputInhibit.p_signal); // add it to the output device

    __outputInhibit.p_device->setBoolInterp_True(__outputInhibit.deviceChannel,INHIBIT_VOLTAGE); // Define desired inhibit voltage response
    __outputInhibit.p_device->setBoolInterp_False(__outputInhibit.deviceChannel,ENABLE_VOLTAGE); // Define desired inhibit voltage response

}
AMC_30A8::AMC_30A8(_outputDevice* vOutDevice, unsigned int outputChannel, _outputDevice* inhibitOutDevice, unsigned int inhibitChannel, string name, ostream* p_outputStream):
            currentOutputDevice( AMC_30A8_MAX_CURRENT, AMC_30A8_NUM_CHANNELS, 0, name, p_outputStream),  deviceInhibit( name + " Inhibit" )
{
    AMC_Initializer(vOutDevice, outputChannel);
    /*** initialize the inhibit output information ***/
    __outputInhibit.deviceChannel		= inhibitChannel;
    __outputInhibit.p_device		= inhibitOutDevice;
    __outputInhibit.p_signal		= deviceInhibit::inhibitSignalPointer();  // add inhibit signal to structure

    __outputInhibit.p_device->addSignal(__outputInhibit.deviceChannel,__outputInhibit.p_signal); // add it to the output device

    __outputInhibit.p_device->setBoolInterp_True(__outputInhibit.deviceChannel,INHIBIT_VOLTAGE); // Define desired inhibit voltage response
    __outputInhibit.p_device->setBoolInterp_False(__outputInhibit.deviceChannel,ENABLE_VOLTAGE); // Define desired inhibit voltage response
}

void AMC_30A8::AMC_Initializer (_outputDevice* vOutDevice, unsigned int outputChannel) {

    _currentSensor = currentSensor(AMC_30A8_AMPS_PER_VOLTS, name() + " Current Measurement", __p_outputStream);
    _Fault = 0;
    _OverCurrent = 0;

    /*** initialize the voltoge output information ***/
    __outputVoltage.deviceChannel		= outputChannel;
    __outputVoltage.p_device		= vOutDevice;
    __outputVoltage.p_signal		= vOutDevice->getSignalPointer(outputChannel);
    if( __outputVoltage.p_signal == 0 ) // no signal exists
    {
        _delete_voltageSignal_onExit = true;
        __outputVoltage.p_signal = new analogTimeStampedSignal(name()+" voltage output", "V", NUM_POINTS, __p_outputStream);  // create a new time stamped analog signal
//		__outputVoltage.p_signal = new analogSignal(name()+" voltage output", "V", NUM_POINTS, __p_outputStream);  // create a new time stamped analog signal

        __outputVoltage.p_device->addSignal(__outputVoltage.deviceChannel,__outputVoltage.p_signal); // add it to the output device

    } else {
        _delete_voltageSignal_onExit = false;
    }
    setPowerLimit(AMC_30A8_POWER_LIMIT,0.0);
    setCurrentLimit(AMC_30A8_MAX_CURRENT);
}

AMC_30A8::~AMC_30A8() {

    if( _delete_voltageSignal_onExit )
        delete __outputVoltage.p_signal;

    if( _Fault ) {
        delete _Fault;
    }

    if( _OverCurrent ) {
        delete _OverCurrent;
    }
}

void AMC_30A8::addCurrentSenseDAQ(analogInputDevice* device, unsigned int inputChannel) {
    device->addInputSignal( inputChannel, &_currentSensor);
}

void AMC_30A8::inhibit()
{
    cout << "Inhibit " << name() << "\t";
    currentsOff();
    deviceInhibit::inhibit();
    __outputInhibit.p_device->write(__outputInhibit.deviceChannel);
    cout << "Done..." << endl;

}
void AMC_30A8::enable()
{
    cout << "Enable " << name() << endl;
    deviceInhibit::enable();
    __outputInhibit.p_device->write(__outputInhibit.deviceChannel);
}

currentSensor* AMC_30A8::getCurrentSensePointer() {
    return &_currentSensor;
}

double AMC_30A8::getMeasured() {
    return _currentSensor.value();
}


logicTimeStampedSignal* AMC_30A8::getFaultPointer() {
    return _Fault;
}
logicTimeStampedSignal* AMC_30A8::getOverCurrentPointer() {
    return _OverCurrent;
}
bool AMC_30A8::deviceInAcceptableRange() {
    /*
    TTL level (+5 V) output becomes high when power devices are disabled due to at least one
    of the following conditions: inhibit, output short circuit, over voltage, over temperature,
    power-up reset.
    */
    if( _Fault != 0 ) {
        return !(_Fault->value().value); // returns true if there is no problem false if there is
    }
    return true; // if no fault signal
}

void AMC_30A8::addFaultSenseDAQ(analogInputDevice* device, unsigned int inputChannel) {
    /*
    TTL level (+5 V) output becomes high when power devices are disabled due to at least one
    of the following conditions: inhibit, output short circuit, over voltage, over temperature,
    power-up reset.
    */

    if( _Fault == 0 )
        _Fault = new logicTimeStampedSignal(this->name() + " Fault Sense",2.5,"",NUM_POINTS,this->__p_outputStream);

    device->addInputSignal( inputChannel, _Fault);
}
void AMC_30A8::addOverCurrentSenseDAQ(analogInputDevice* device, unsigned int inputChannel) {

    /*
    This transistor output becomes high if RMS current (in any phase) exceeds RMS current
    limit. This creates a fault condition. Drive will re-enable when the RMS current returns to
    values within the RMS current limit range.
    */

    if( _OverCurrent == 0 )
        _OverCurrent = new logicTimeStampedSignal(this->name() + " Over Current Sense",2.5,"",NUM_POINTS,this->__p_outputStream);

    device->addInputSignal( inputChannel, _OverCurrent);
}
