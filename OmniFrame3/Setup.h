#ifndef SETUP_H
#define SETUP_H

//Include files purely for Omnimagnet Setup
#include "Devices/MultipleOmnimagnetSetup.h"
#include "Devices/MultipleOmnimagnetSystem.h"

#include "Producers/MagneticToolProducer.h"
#include "Producers/Configuration5DOF_FromFile.h"
#include "Producers/Configuration5DOF_Producer.h"
#include "Producers/ConstantPositionProducer.h"
#include "Producers/TransformedConfiguration5DOF_Producer.h"
#include "Producers/SinglePerminantMagnetTool.h"
#include "Producers/TwoOmnimagnet_SingularDirectionProducer.h"
#include "Producers/PositionProducer.h"

#include "Controls/DiscreteController.h"

#include <ArduinoSetup.h>
#include <QTimer>

class Setup: public QObject
{
    Q_OBJECT

public:
    Setup();
    ~Setup();

    // Omnimagnets and System
    MultipleOmnimagnetSetup systemSetup;
    deviceInhibit* systemInhibit;
    autoInhibit* systemAutoInhibit;
    MultipleOmnimagnetSystem* omnimagSystem;

    ArduinoSetup* Arduinos;

    // Producers
    DirectionProducer* goalDirection;
    ConstantPositionProducer* toolLocation;
    MagneticToolProducer* toolProducer;
    PositionProducer* goalLocation;
    TransformedConfiguration5DOF_producer* goalPose;
    SinglePerminantMagnetTool* magneticTool;
    TwoOmnimagnet_SingularDirectionProducer* goodDirection;

    // Transforms
    HomogeneousTransform TrajectoryToWorld;

    // Trajectory
    Configuration5DOF_Producer* trajectory_PositionProducer;

    // Controller
    DiscreteVectorController* positionController;

    // MISC Sensors & Signals
    analogVectorTimeStampedSignal* desiredForce;
    AD_IO::analogSensor*  p_powerSignal;

    // Data logging
    fileLogger* fileLog;

    //Functions
    void OFF();
    QTimer updateTimer;

    //LAN
    ForceVector toolWeight;
    DipoleMoment toolDipoleMoment;
    double omni_orient[6];

private slots:

    void updateConfiguration();
};

#endif // SETUP_H
