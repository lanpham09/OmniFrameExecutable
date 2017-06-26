#include "MultipleOmnimagnetSetup.h"

MultipleOmnimagnetSetup::MultipleOmnimagnetSetup(std::string logDirectoryName, bool enable_auto_inhibit_functionality):
    AD_IO::deviceInhibit("System Inhibit")
{
    //systemInhibit = new AD_IO::deviceInhibit("System Inhibit");
    systemAutoInhibit = new AD_IO::autoInhibit( this, 100 ); // check all Omnimagnets (and other devices added)
    //    every 100 ms to see if in range inhibit otherwise

    // *********** NOTE: THESE INHIBITS DO NOT ACTUALLY CORRESPOND TO THE APPROPRIATE DRIVES. THEY ARE ATTACHED RANDOMLY ******** //
    int inhibitPin = 26;
    int inhibitPin2 = 25;

    currentDrive[0]  = new AMC_30A8(&voltageDrive,  1, inhibitPin, this, "Omnimagnet0 Inner Current Drive" );
    currentDrive[1]  = new AMC_30A8(&voltageDrive,  9, inhibitPin, this, "Omnimagnet0 Middle Current Drive" );
    currentDrive[2]  = new AMC_30A8(&voltageDrive, 17, inhibitPin, this, "Omnimagnet0 Outer Current Drive" );

    currentDrive[3]  = new AMC_30A8(&voltageDrive,  2, inhibitPin, this, "Omnimagnet1 Inner Current Drive" );
    currentDrive[4]  = new AMC_30A8(&voltageDrive, 10, inhibitPin, this, "Omnimagnet1 Middle Current Drive" );
    currentDrive[5]  = new AMC_30A8(&voltageDrive, 18, inhibitPin, this, "Omnimagnet1 Outer Current Drive" );

    currentDrive[6]  = new AMC_30A8(&voltageDrive,  3, inhibitPin, this, "Omnimagnet2 Inner Current Drive" );
    currentDrive[7]  = new AMC_30A8(&voltageDrive, 11, inhibitPin, this, "Omnimagnet2 Middle Current Drive" );
    currentDrive[8]  = new AMC_30A8(&voltageDrive, 19, inhibitPin, this, "Omnimagnet2 Outer Current Drive" );

    currentDrive[9]  = new AMC_30A8(&voltageDrive,  4, inhibitPin2, this, "Omnimagnet3 Inner Current Drive" );
    currentDrive[10] = new AMC_30A8(&voltageDrive, 12, inhibitPin2, this, "Omnimagnet3 Middle Current Drive" );
    currentDrive[11] = new AMC_30A8(&voltageDrive, 20, inhibitPin2, this, "Omnimagnet3 Outer Current Drive" );

    currentDrive[12] = new AMC_30A8(&voltageDrive,  5, inhibitPin2, this, "Omnimagnet4 Inner Current Drive" );
    currentDrive[13] = new AMC_30A8(&voltageDrive, 13, inhibitPin2, this, "Omnimagnet4 Middle Current Drive" );
    currentDrive[14] = new AMC_30A8(&voltageDrive, 21, inhibitPin2, this, "Omnimagnet4 Outer Current Drive" );

    currentDrive[15] = new AMC_30A8(&voltageDrive,  6, inhibitPin2, this, "Omnimagnet5 Inner Current Drive" );
    currentDrive[16] = new AMC_30A8(&voltageDrive, 14, inhibitPin2, this, "Omnimagnet5 Middle Current Drive" );
    currentDrive[17] = new AMC_30A8(&voltageDrive, 22, inhibitPin2, this, "Omnimagnet5 Outer Current Drive" );

    currentDrive[18] = new AMC_30A8(&voltageDrive,  7, inhibitPin2, this, "Omnimagnet6 Inner Current Drive" );
    currentDrive[19] = new AMC_30A8(&voltageDrive, 15, inhibitPin2, this, "Omnimagnet6 Middle Current Drive" );
    currentDrive[20] = new AMC_30A8(&voltageDrive, 23, inhibitPin2, this, "Omnimagnet6 Outer Current Drive" );

    for( int i=0; i<NUM_VECTOR_OUT_DEVICE; i++ )
    {
        vectorOutDevice[i] =  new AD_IO::vectorCurrentOutput(currentDrive[i*3+0],0, currentDrive[i*3+1], 0, currentDrive[i*3+2],0, true, "Vector Current Drive for Omnimagnet " + to_string<int>(i) );
        vectorOutDevice[i]->setPowerLimit(800);
    }

    //#0 is the big magnet
    omnimagnet0 = new Omnimagnet(Math::Distance(4, INCH).value(METER), // core radious
                                 Math::Vector(25.1, 25.8, 26.3), // dipole per Amp (inner,middle,outer)
                                 Math::Vector(5.84,5.96,5.78), // coil resistance (inner,middle,outer)
                                 vectorOutDevice[0], // vector current output device
                                 0, // Designator
                                 Configuration6DOF( Position(Zero(),WORLD_FRAME,METER), Orientation(Math::Identity(),WORLD_FRAME, OMNIMAGNET_FRAME) ), // pose of magnet
                                 "Omnimagnet" );

    omnimagnet1 = new Omnimagnet(Math::Distance(2.920, INCH).value(METER), // core radious
                                 Math::Vector(5.3527, 5.3342, 5.4719), // dipole per Amp (inner,middle,outer)
                                 Math::Vector(1.6858, 1.7067, 1.7136), // coil resistance (inner,middle,outer)
                                 vectorOutDevice[1], // vector current output device
                                 1, // Designator
                                 Configuration6DOF( Position(Zero(),WORLD_FRAME,METER), Orientation(Math::Identity(),WORLD_FRAME, OMNIMAGNET_FRAME) ), // pose of magnet
                                 "Omnimagnet" );

    omnimagnet2 = new Omnimagnet(Math::Distance(2.920, INCH).value(METER), // core radious
                                 Math::Vector(5.3527, 5.3342, 5.4719), // dipole per Amp (inner,middle,outer)
                                 Math::Vector(1.6858, 1.7067, 1.7136), // coil resistance (inner,middle,outer)
                                 vectorOutDevice[2], // vector current output device
                                 2, // Designator
                                 Configuration6DOF( Position(Zero(),WORLD_FRAME,METER), Orientation(Math::Identity(),WORLD_FRAME, OMNIMAGNET_FRAME) ), // pose of magnet
                                 "Omnimagnet" );

    omnimagnet3 = new Omnimagnet(Math::Distance(2.920, INCH).value(METER), // core radious
                                 Math::Vector(5.3527, 5.3342, 5.4719), // dipole per Amp (inner,middle,outer)
                                 Math::Vector(1.6858, 1.7067, 1.7136), // coil resistance (inner,middle,outer)
                                 vectorOutDevice[3], // vector current output device
                                 3, // Designator
                                 Configuration6DOF( Position(Zero(),WORLD_FRAME,METER), Orientation(Math::Identity(),WORLD_FRAME, OMNIMAGNET_FRAME) ), // pose of magnet
                                 "Omnimagnet" );

    omnimagnet4 = new Omnimagnet(Math::Distance(2.920, INCH).value(METER), // core radious
                                 Math::Vector(5.3527, 5.3342, 5.4719), // dipole per Amp (inner,middle,outer)
                                 Math::Vector(1.6858, 1.7067, 1.7136), // coil resistance (inner,middle,outer)
                                 vectorOutDevice[4], // vector current output device
                                 4, // Designator
                                 Configuration6DOF( Position(Zero(),WORLD_FRAME,METER), Orientation(Math::Identity(),WORLD_FRAME, OMNIMAGNET_FRAME) ), // pose of magnet
                                 "Omnimagnet" );

    omnimagnet5 = new Omnimagnet(Math::Distance(2.920, INCH).value(METER), // core radious
                                 Math::Vector(5.3527, 5.3342, 5.4719), // dipole per Amp (inner,middle,outer)
                                 Math::Vector(1.6858, 1.7067, 1.7136), // coil resistance (inner,middle,outer)
                                 vectorOutDevice[5], // vector current output device
                                 5, // Designator
                                 Configuration6DOF( Position(Zero(),WORLD_FRAME,METER), Orientation(Math::Identity(),WORLD_FRAME, OMNIMAGNET_FRAME) ), // pose of magnet
                                 "Omnimagnet" );

    omnimagnet6 = new Omnimagnet(Math::Distance(2.920, INCH).value(METER), // core radious
                                 Math::Vector(5.3527, 5.3342, 5.4719), // dipole per Amp (inner,middle,outer)
                                 Math::Vector(1.6858, 1.7067, 1.7136), // coil resistance (inner,middle,outer)
                                 vectorOutDevice[6], // vector current output device
                                 6, // Designator
                                 Configuration6DOF( Position(Zero(),WORLD_FRAME,METER), Orientation(Math::Identity(),WORLD_FRAME, OMNIMAGNET_FRAME) ), // pose of magnet
                                 "Omnimagnet" );

    if( enable_auto_inhibit_functionality )
    {
        systemAutoInhibit->addAutoInhibitDevice(omnimagnet0);
        systemAutoInhibit->addAutoInhibitDevice(omnimagnet1);
        systemAutoInhibit->addAutoInhibitDevice(omnimagnet2);
        systemAutoInhibit->addAutoInhibitDevice(omnimagnet3);
        systemAutoInhibit->addAutoInhibitDevice(omnimagnet4);
        systemAutoInhibit->addAutoInhibitDevice(omnimagnet5);
        systemAutoInhibit->addAutoInhibitDevice(omnimagnet6);
    }


    dataLogger = new AD_IO::fileLogger(logDirectoryName);

    // Add Dipole Moment History to file log
    dataLogger->addSignal(omnimagnet0->getVectorSignalPointer()); // Dipole Moment
    dataLogger->addSignal(omnimagnet1->getVectorSignalPointer()); // Dipole Moment
    dataLogger->addSignal(omnimagnet2->getVectorSignalPointer()); // Dipole Moment
    dataLogger->addSignal(omnimagnet3->getVectorSignalPointer()); // Dipole Moment
    dataLogger->addSignal(omnimagnet4->getVectorSignalPointer()); // Dipole Moment
    dataLogger->addSignal(omnimagnet5->getVectorSignalPointer()); // Dipole Moment
    dataLogger->addSignal(omnimagnet6->getVectorSignalPointer()); // Dipole Moment

    //systemInhibit->enable();

    inhibit();

}

MultipleOmnimagnetSetup::~MultipleOmnimagnetSetup()
{
    delete  dataLogger;
    delete  omnimagnet6;
    delete  omnimagnet5;
    delete  omnimagnet4;
    delete  omnimagnet3;
    delete  omnimagnet2;
    delete  omnimagnet1;
    delete  omnimagnet0;

    for( int i=0; i<NUM_VECTOR_OUT_DEVICE; i++ )
    {
        delete vectorOutDevice[i];
    }

    for( int i=0; i<NUM_CURRENT_DRIVES; i++ )
    {
        delete currentDrive[i];
    }

    delete systemAutoInhibit;
}

Omnimagnet* MultipleOmnimagnetSetup::operator()(omnimagnetNumber num)
{
    switch(num)
    {
    case OmniMag0:
        return omnimagnet0;
        break;
    case OmniMag1:
        return omnimagnet1;
        break;
    case OmniMag2:
        return omnimagnet2;
        break;
    case OmniMag3:
        return omnimagnet3;
        break;
    case OmniMag4:
        return omnimagnet4;
        break;
    case OmniMag5:
        return omnimagnet5;
        break;
    case OmniMag6:
        return omnimagnet6;
        break;

    }
    return 0;
}
