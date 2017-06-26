#ifndef AMC_30A8_H
#define AMC_30A8_H

#include "AD_IO/AD_IO.h"

using namespace AD_IO;


#define AMC_30A8_MAX_CURRENT 15.0 // Amps Continuous
#define AMC_30A8_NUM_CHANNELS 1
#define AMC_30A8_AMPS_PER_VOLTS 3.8
#define AMC_30A8_VOLTS_PER_AMP 0.6666666667 // Need to Calibrate Drive manually by adjusting pot 3. Set to 10V/15A so we never are in peak mode...

#define AMC_30A8_POWER_LIMIT 1140.0

#define INHIBIT_VOLTAGE 0
#define ENABLE_VOLTAGE 5

class AMC_30A8:public currentOutputDevice, public deviceInhibit, public autoInhibitDevice {
private:
    struct signalDef {
        signal* p_signal;
        _outputDevice* p_device;
        unsigned int deviceChannel;
    } __outputVoltage, __outputInhibit;

    currentSensor _currentSensor;

    logicTimeStampedSignal* _Fault;
    logicTimeStampedSignal* _OverCurrent;

    bool _delete_voltageSignal_onExit;

    void AMC_Initializer(_outputDevice* vOutDevice, unsigned int outputChannel);

protected:
    bool writeCurrents(const vector<writeValue>& channelsAndValues);

public:
    AMC_30A8(_outputDevice* vOutDevice, unsigned int outputChannel, unsigned int inhibitChannel, deviceInhibit* p_systemInhibit, string name = "", ostream* p_outputStream = &cout);
    AMC_30A8(_outputDevice* vOutDevice, unsigned int outputChannel, unsigned int inhibitChannel, string name = "", ostream* p_outputStream = &cout);
    AMC_30A8(_outputDevice* vOutDevice, unsigned int outputChannel, _outputDevice* inhibitOutDevice, unsigned int inhibitChannel, deviceInhibit* p_systemInhibit, string name = "", ostream* p_outputStream = &cout);
    AMC_30A8(_outputDevice* vOutDevice, unsigned int outputChannel, _outputDevice* inhibitOutDevice, unsigned int inhibitChannel, string name = "", ostream* p_outputStream = &cout);
    ~AMC_30A8();

    void inhibit();
    void enable();

    void addCurrentSenseDAQ(analogInputDevice* device, unsigned int inputChannel);
    void addFaultSenseDAQ(analogInputDevice* device, unsigned int inputChannel);
    void addOverCurrentSenseDAQ(analogInputDevice* device, unsigned int inputChannel);

    currentSensor*	getCurrentSensePointer();

    logicTimeStampedSignal* getFaultPointer();
    logicTimeStampedSignal* getOverCurrentPointer();
    bool deviceInAcceptableRange();

    analogTimeStampedSignal* getSignalPointer() { return (analogTimeStampedSignal*)currentOutputDevice::getSignalPointer(0);}

    double getMeasured();
};


#endif
