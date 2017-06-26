#ifndef OMNIMAGNET_H
#define OMNIMAGNET_H


#include <string>
#include <iostream>

#include "AD_IO/AD_IO.h"
#include "AD_IO/io_object.h"
#include "Utilities/Math.h"
#include "Utilities/Magnetics.h"
#include "Utilities/Timer.h"
#include "Producers/FieldProducer.h"
#include "Producers/ConstMomentFieldProducer.h"
#include "Producers/MomentProducer.h"
#include "Producers/RotatingMomentProducer.h"
#include "Producers/Configuration6DOF_Producer.h"
#include "Producers/VectorTemperature_Producer.h"


using namespace Math;
using namespace AD_IO;
using namespace Timing;
using namespace std;

class Omnimagnet:private Timer, public device, public Configuration6DOF_Producer, public FieldProducer, public MomentProducer,
        public ConstMomentFieldProducer, public autoInhibitDevice
{
private:
    static const unsigned int __numCoils = 3;

    analogVectorTimeStampedSignal* __p_dipoleMomentVectorHistory; // dipole moment signals in world frame (0->x,1->y,2->z)
    analogSignal* __p_dipoleMomentDefault[3]; // dipole moment signals in world frame (0->x,1->y,2->z)

    vectorCurrentOutput* __p__CurrentOutputDevice; // Current output device

    VectorTemperature_Producer* __p_coilTempProducer;

    Vector tempCurrentCommanded;

    Matrix3x3 __amp_per_dipoleMoment;
    Matrix3x3 __coilResistances;

    Distance __coreRadius;// core radius in meters

    static const int MAX_TEMP = 100; // maximum allowed temp for auto inhibt is 100C

    bool delete_ON_EXIT__p__CurrentOutputDevice;

    void initializer(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, vectorCurrentOutput* p_curentOutputDevice, string name, ostream* p_outputStream);

public:
    enum Coil{ INNER, MIDDLE, OUTER };

    Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, vectorCurrentOutput* p_curentOutputDevice, unsigned int designator = 0, const Configuration6DOF& pose = Configuration6DOF( Position(Zero(),OMNIMAGNET_FRAME,METER), Orientation(Math::Identity(),OMNIMAGNET_FRAME,OMNIMAGNET_FRAME) ), string name = "Omnimagnet", ostream* p_outputStream = &cout );
    Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, vectorCurrentOutput* p_curentOutputDevice, VectorTemperature_Producer* tempProducerPointer, unsigned int designator = 0, const Configuration6DOF& pose = Configuration6DOF( Position(Zero(),OMNIMAGNET_FRAME,METER), Orientation(Math::Identity(),OMNIMAGNET_FRAME,OMNIMAGNET_FRAME) ), string name = "Omnimagnet", ostream* p_outputStream = &cout );

    Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, currentOutputDevice* p_curentOutputDevice_INNER, unsigned int channelINNER, currentOutputDevice* p_curentOutputDevice_MIDDLE, unsigned int channelMIDDLE, currentOutputDevice* p_curentOutputDevice_OUTER, unsigned int channelOUTER, unsigned int designator = 0, const Configuration6DOF& pose = Configuration6DOF( Position(Zero(),OMNIMAGNET_FRAME,METER), Orientation(Math::Identity(),OMNIMAGNET_FRAME,OMNIMAGNET_FRAME) ), string name = "Omnimagnet", ostream* p_outputStream = &cout );
    Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, currentOutputDevice* p_curentOutputDevice_INNER, unsigned int channelINNER, currentOutputDevice* p_curentOutputDevice_MIDDLE, unsigned int channelMIDDLE, currentOutputDevice* p_curentOutputDevice_OUTER, unsigned int channelOUTER, VectorTemperature_Producer* tempProducerPointer, unsigned int designator = 0, const Configuration6DOF& pose = Configuration6DOF( Position(Zero(),OMNIMAGNET_FRAME,METER), Orientation(Math::Identity(),OMNIMAGNET_FRAME,OMNIMAGNET_FRAME) ), string name = "Omnimagnet", ostream* p_outputStream = &cout );

    Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, currentOutputDevice* p_curentOutputDevice_INNER, unsigned int channelINNER, currentOutputDevice* p_curentOutputDevice_MIDDLE, unsigned int channelMIDDLE, currentOutputDevice* p_curentOutputDevice_OUTER, unsigned int channelOUTER, unsigned int designator, const Configuration6DOF_Producer& poseProducer, string name = "Omnimagnet", ostream* p_outputStream = &cout );
    Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, currentOutputDevice* p_curentOutputDevice_INNER, unsigned int channelINNER, currentOutputDevice* p_curentOutputDevice_MIDDLE, unsigned int channelMIDDLE, currentOutputDevice* p_curentOutputDevice_OUTER, unsigned int channelOUTER, VectorTemperature_Producer* tempProducerPointer, unsigned int designator, const Configuration6DOF_Producer& poseProducer, string name = "Omnimagnet", ostream* p_outputStream = &cout );


    ~Omnimagnet();

    signal* getVectorSignalPointer() const;
    bool addLogger(unsigned int channel, device* p_SignalDataLogger, unsigned int loggerChannel=0);
    bool removeLogger(unsigned int channel, device* p_SignalDataLogger );

    bool addSignal( unsigned int channel, signal* p_signal );
    void removeSignal( unsigned int channel );
    int removeSignal( signal* p_signal );

    bool OFF(); // sets the dipole moment to zero

    Vector commandedCurrent(); // returns the current command sent to the vectorCurrentOutput device

    double returnTemperature(Coil coil) const; // returns the average temperature of the coil requested; returns 0 if no measurement is setup
    Vector returnTemperature() const; // returns the average temperature of the coil requested; returns (0,0,0) if no measurement is setup
    vectorSensor* getTemperaturePointer( ); // returns the pointer to the VectorTemperatureProducer history
    bool setupTempMeasurement(VectorTemperature_Producer* tempProducer); // adds a thermocouple measurement device for a specific TC

    // Moment Producer Functions
    bool dipoleMoment(const DipoleMomentVector& moment);  // commands a dipole moment
    DipoleMomentVector dipoleMoment() const; // returs the dipole moment last commanded in the world frame

    // Field Producer Functions
    bool applyField(const MagneticFieldVector& field, const Position& field_position); // commands a field value at a global location (Teslas and Meters)
    MagneticFieldVector currentField(const Position& position); // returs the field value at a point specified based on the current dipole moment command

    // Force Producer Functions
    bool forceAtPoint( const Math::ForceVector& force, const MagneticTool& endEffector);
    ForceVector forceAtPoint( const MagneticTool& endEffector );

    // Torque Producer Functions
    bool torqueAtPoint( const Math::TorqueVector& torque, const MagneticTool& endEffector);
    TorqueVector torqueAtPoint( const MagneticTool& endEffector );

    // Const Moment Field Producer Function
    bool fieldAtPoint( const Math::Direction& fieldDirection, const Position& field_position, DipoleMoment momentStrength );

    // Auto Inhibit Device Function
    bool deviceInAcceptableRange();
/*
    // PositionProducer Function
    bool getPosition(Math::Position &position, distanceUnits units = DIST_NATIVE);

    bool getOrientation(Math::Orientation &orientation) const;
    bool getConfiguration6DOF( Math::Configuration6DOF & configuration) const;

    bool setPosition(const Math::Position &position);
    bool setOrientation(const Math::Matrix3x3 & orientation );
    bool setConfiguration6Dof(const Math::Configuration6DOF & configuration);
*/

    void changeConfiguration6DOF_Producer( const Configuration6DOF_Producer& newProducer );

    Frame getFrame() const;



    // Get Omnimagnet Properties
    Distance getCoreRadius() const;
    Math::Matrix3x3 getDipoleMomentPerAmp();
    Math::Matrix3x3 getCoilResistanceMatrix() const;
    Math::Matrix3x3 getWorldResistanceMatrix();
    Math::Matrix3x3 getWorldResistanceMatrix_SquareRoot();
    Vector getMaxMoment();


};

#endif
