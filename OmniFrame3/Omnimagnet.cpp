#include "Devices/Omnimagnet.h"
#include "Utilities/utilities.h"
#include <iostream>

#define NUM_POINTS 2000

//inputs: (core radio, dipole/amp, Math::Vector(coil resistance), pointers to vector current output deivces, channel #, Configuration, name, ostream output)
Omnimagnet::Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, vectorCurrentOutput* p_curentOutputDevice, unsigned int designator, const Configuration6DOF& pose, string name, ostream* p_outputStream):
    device(__numCoils, designator, name, io_object::type::DOUBLE, p_outputStream),
    Configuration6DOF_Producer( pose, name + " Position" )
{
    initializer(coreRadius, DipolePerAmp, coilResistances, p_curentOutputDevice, name, p_outputStream);

}

Omnimagnet::Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, vectorCurrentOutput* p_curentOutputDevice, VectorTemperature_Producer* tempProducerPointer, unsigned int designator, const Configuration6DOF& pose, string name, ostream* p_outputStream):
    device(__numCoils, designator, name, io_object::type::DOUBLE, p_outputStream),
    Configuration6DOF_Producer( pose, name + " Position" )
{
    initializer(coreRadius, DipolePerAmp, coilResistances, p_curentOutputDevice, name, p_outputStream);
    __p_coilTempProducer = tempProducerPointer;
}

Omnimagnet::Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, currentOutputDevice* p_curentOutputDevice_INNER, unsigned int channelINNER, currentOutputDevice* p_curentOutputDevice_MIDDLE, unsigned int channelMIDDLE, currentOutputDevice* p_curentOutputDevice_OUTER, unsigned int channelOUTER, unsigned int designator, const Configuration6DOF& pose, string name, ostream* p_outputStream ):
    device(__numCoils, designator, name, io_object::type::DOUBLE, p_outputStream),
    Configuration6DOF_Producer( pose, name + " Position" )
{
    // create vectorCurrentOutput pointer
    __p__CurrentOutputDevice = new vectorCurrentOutput( p_curentOutputDevice_INNER, channelINNER, p_curentOutputDevice_MIDDLE, channelMIDDLE, p_curentOutputDevice_OUTER, channelOUTER, true, name+" Current Vector Output");

    initializer(coreRadius, DipolePerAmp, coilResistances, __p__CurrentOutputDevice, name, p_outputStream);

    delete_ON_EXIT__p__CurrentOutputDevice = true;
}

Omnimagnet::Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, currentOutputDevice* p_curentOutputDevice_INNER, unsigned int channelINNER, currentOutputDevice* p_curentOutputDevice_MIDDLE, unsigned int channelMIDDLE, currentOutputDevice* p_curentOutputDevice_OUTER, unsigned int channelOUTER, VectorTemperature_Producer* tempProducerPointer, unsigned int designator, const Configuration6DOF& pose, string name, ostream* p_outputStream ):
    device(__numCoils, designator, name, io_object::type::DOUBLE, p_outputStream),
    Configuration6DOF_Producer( pose, name + " Position" )
{
    // create vectorCurrentOutput pointer
    __p__CurrentOutputDevice = new vectorCurrentOutput( p_curentOutputDevice_INNER, channelINNER, p_curentOutputDevice_MIDDLE, channelMIDDLE, p_curentOutputDevice_OUTER, channelOUTER, true, name+" Current Vector Output");

    initializer(coreRadius, DipolePerAmp, coilResistances, __p__CurrentOutputDevice, name, p_outputStream);

    delete_ON_EXIT__p__CurrentOutputDevice = true;

    __p_coilTempProducer = tempProducerPointer;

}

Omnimagnet::Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, currentOutputDevice* p_curentOutputDevice_INNER, unsigned int channelINNER, currentOutputDevice* p_curentOutputDevice_MIDDLE, unsigned int channelMIDDLE, currentOutputDevice* p_curentOutputDevice_OUTER, unsigned int channelOUTER, unsigned int designator, const Configuration6DOF_Producer& poseProducer, string name, ostream* p_outputStream ):
    device(__numCoils, designator, name, io_object::type::DOUBLE, p_outputStream),
    Configuration6DOF_Producer( poseProducer )
{
    // create vectorCurrentOutput pointer
    __p__CurrentOutputDevice = new vectorCurrentOutput( p_curentOutputDevice_INNER, channelINNER, p_curentOutputDevice_MIDDLE, channelMIDDLE, p_curentOutputDevice_OUTER, channelOUTER, true, name+" Current Vector Output");

    initializer(coreRadius, DipolePerAmp, coilResistances, __p__CurrentOutputDevice, name, p_outputStream);

    delete_ON_EXIT__p__CurrentOutputDevice = true;
}

Omnimagnet::Omnimagnet(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, currentOutputDevice* p_curentOutputDevice_INNER, unsigned int channelINNER, currentOutputDevice* p_curentOutputDevice_MIDDLE, unsigned int channelMIDDLE, currentOutputDevice* p_curentOutputDevice_OUTER, unsigned int channelOUTER, VectorTemperature_Producer* tempProducerPointer, unsigned int designator, const Configuration6DOF_Producer& poseProducer, string name, ostream* p_outputStream):
    device(__numCoils, designator, name, io_object::type::DOUBLE, p_outputStream),
    Configuration6DOF_Producer( poseProducer )
{
    // create vectorCurrentOutput pointer
    __p__CurrentOutputDevice = new vectorCurrentOutput( p_curentOutputDevice_INNER, channelINNER, p_curentOutputDevice_MIDDLE, channelMIDDLE, p_curentOutputDevice_OUTER, channelOUTER, true, name+" Current Vector Output");

    initializer(coreRadius, DipolePerAmp, coilResistances, __p__CurrentOutputDevice, name, p_outputStream);

    delete_ON_EXIT__p__CurrentOutputDevice = true;

    __p_coilTempProducer = tempProducerPointer;
}

//********************
//  Functions
//********************

void Omnimagnet::initializer(Distance coreRadius, const Vector& DipolePerAmp, const Vector& coilResistances, vectorCurrentOutput* p_curentOutputDevice, string name, ostream* p_outputStream) {

    __p_coilTempProducer = 0;

    __amp_per_dipoleMoment = (DipolePerAmp.elementWiseInverse()).asDiagonal();

    __coreRadius = coreRadius;
    __p__CurrentOutputDevice = p_curentOutputDevice;

    __p__CurrentOutputDevice->setChannelResistance(coilResistances);

    __coilResistances = coilResistances.asDiagonal();

    // create default world dipole moment signals
    __p_dipoleMomentDefault[0] =  new analogTimeStampedSignal(name + " " + to_string<int>(__designator) + " Dipole Moment X", "A-m^2", 2, p_outputStream);
    __p_dipoleMomentDefault[1] =  new analogTimeStampedSignal(name + " " + to_string<int>(__designator) + " Dipole Moment Y", "A-m^2", 2, p_outputStream);
    __p_dipoleMomentDefault[2] =  new analogTimeStampedSignal(name + " " + to_string<int>(__designator) + " Dipole Moment Z", "A-m^2", 2, p_outputStream);
    __p_dipoleMomentVectorHistory = new analogVectorTimeStampedSignal(name + " " + to_string<int>(__designator) + " Dipole Moment", "A-m^2", 500, p_outputStream);
    /*
    __p_dipoleMomentDefault[0] =  new analogSignal(name + " Dipole Moment X", "A-m^2", NUM_POINTS, p_outputStream);
    __p_dipoleMomentDefault[1] =  new analogSignal(name + " Dipole Moment Y", "A-m^2", NUM_POINTS, p_outputStream);
    __p_dipoleMomentDefault[2] =  new analogSignal(name + " Dipole Moment Z", "A-m^2", NUM_POINTS, p_outputStream);
*/
    addSignal(0, __p_dipoleMomentDefault[0]);
    addSignal(1, __p_dipoleMomentDefault[1]);
    addSignal(2, __p_dipoleMomentDefault[2]);
    this->io_object::attachObject(__p_dipoleMomentVectorHistory);

    delete_ON_EXIT__p__CurrentOutputDevice = false;
}

Omnimagnet::~Omnimagnet() {

    for(unsigned int i=0; i<__numCoils; i++) {
        if( __p_dipoleMomentDefault[i] != NULL )  {
            device::removeSignal(__p_dipoleMomentDefault[i]);
            delete __p_dipoleMomentDefault[i]; // delete all default moment signals
        }
    }

    delete __p_dipoleMomentVectorHistory;


    if(delete_ON_EXIT__p__CurrentOutputDevice)
    {
        if( __p__CurrentOutputDevice )
            delete __p__CurrentOutputDevice;
    }
}

signal* Omnimagnet::getVectorSignalPointer() const
{
    return __p_dipoleMomentVectorHistory;
}

bool Omnimagnet::addLogger(unsigned int, device* p_SignalDataLogger, unsigned int loggerChannel) { // for an output channel

    return p_SignalDataLogger->addSignal(loggerChannel,__p_dipoleMomentVectorHistory);
}

bool Omnimagnet::removeLogger(unsigned int, device* p_SignalDataLogger) { // for an output channel

    return p_SignalDataLogger->removeSignal(__p_dipoleMomentVectorHistory);

}

bool Omnimagnet::addSignal( unsigned int channel, signal* p_signal ) {

    if(inRange_Array<unsigned int>(channel, 0, __numChannels)) {
        if( __p_channelVector[channel] != NULL )
            device::removeSignal( channel );

        return device::addSignal(channel, p_signal);
    } else {
        *__p_outputStream << name() + ":\t"+to_string((long long) channel) + " does not correspond to a Cartesian direction where x->0, y->1, z->2." << endl;
        return false;
    }
}

void Omnimagnet::removeSignal( unsigned int channel )
{
    if(inRange_Array<unsigned int>(channel, 0, __numChannels)) {
        addSignal(channel, __p_dipoleMomentDefault[channel]);
    }
}

int Omnimagnet::removeSignal( signal* p_signal )
{
    int channel = device::removeSignal(p_signal);
    addSignal(channel, __p_dipoleMomentDefault[channel]);
    return channel;
}

bool Omnimagnet::dipoleMoment(const DipoleMomentVector& moment)
{
    // commands a dipole moment

    for(unsigned int i = 0; i < __numCoils; i++)
        __p_channelVector[i]->append(moment(i)); // add moment to moment signals



    Vector coilCurrents;
    coilCurrents = (returnOrientation()*moment).asVector(AMP_METER_SQ);// Rotate moment into Omnimagnet Frame

    coilCurrents = 	__amp_per_dipoleMoment*coilCurrents; // calcualte currents from moments

    //cout << "Desired Moment: " << moment << " Moment in Omni Frame: " << returnOrientation()*moment << " As Vector " << (returnOrientation()*moment).asVector(AMP_METER_SQ)<< " as Current " << __amp_per_dipoleMoment*coilCurrents << endl;


    __p_dipoleMomentVectorHistory->append(moment.asVector());

    tempCurrentCommanded = coilCurrents;

    return __p__CurrentOutputDevice->Command(coilCurrents); // command currents
}

bool Omnimagnet::OFF()
{
    // sets the dipole moment to zero
    return dipoleMoment( DipoleMomentVector(0,0,0,getFrame()) );
}

double Omnimagnet::returnTemperature(Coil coil) const
{
    // returns the average temperature of the coil requested; returns -9999 if no measurement is setup

    double temperature = 0;
    if( __p_coilTempProducer )
    {
        Math::Vector temp = __p_coilTempProducer->returnTemperature();
        switch(coil)
        {
        case INNER:
            temperature = temp.x_c();
            break;
        case MIDDLE:
            temperature = temp.y_c();
            break;
        case OUTER:
            temperature = temp.z_c();
            break;
        }
    }

    return temperature; // average temp over coil
}

Vector Omnimagnet::returnTemperature() const
{
    if( __p_coilTempProducer )
        return __p_coilTempProducer->returnTemperature();

    return Math::Zero();
}

Vector Omnimagnet::commandedCurrent()
{
    // returns the current command sent to the vectorCurrentOutput device
    return __p__CurrentOutputDevice->getCommand();
}


DipoleMomentVector Omnimagnet::dipoleMoment() const
{
    // returs the dipole moment last commanded in the world frame
    return DipoleMomentVector(__p_dipoleMomentVectorHistory->value().value,getFrame());
}


MagneticFieldVector Omnimagnet::currentField(const Position& point_position)
{
    // returs the field value at a point specified based on the current dipole moment command

    return Magnetics::dipoleFieldAtPoint(dipoleMoment(),point_position-returnPosition());
}


vectorSensor* Omnimagnet::getTemperaturePointer( )
{
    // returns the pointer to the thermocouple singal for the coil speified and TC# (0 or 1)
    if( __p_coilTempProducer )
        //return __p_coilTempProducer->getTemperatureHistoryPointer();
    return 0;
}

bool Omnimagnet::setupTempMeasurement(VectorTemperature_Producer *tempProducerPtr)
{
    __p_coilTempProducer = tempProducerPtr;
}

Distance Omnimagnet::getCoreRadius() const
{
    return __coreRadius;
}

Math::Matrix3x3 Omnimagnet::getDipoleMomentPerAmp()
{
    Math::Matrix3x3 momentPerAmp;
    momentPerAmp << 1.0/__amp_per_dipoleMoment(0,0), 0, 0,
            0, 1.0/__amp_per_dipoleMoment(1,1), 0,
            0, 0, 1.0/__amp_per_dipoleMoment(2,2);

    return (returnOrientation().inverse()) * momentPerAmp;
}

Math::Matrix3x3 Omnimagnet::getCoilResistanceMatrix() const
{
    return __coilResistances;
}

Math::Matrix3x3 Omnimagnet::getWorldResistanceMatrix()
{
    Math::Matrix3x3 returnMatrix = returnOrientation().inverse()*__coilResistances;

    for( unsigned int i=0; i<3; i++ )
        for( unsigned int j=0; j<3; j++ )
            returnMatrix(i,j) = abs(returnMatrix(i,j));//Guarantee that resistances are positive!

    return returnMatrix;
}
Math::Matrix3x3 Omnimagnet::getWorldResistanceMatrix_SquareRoot()
{
    Math::Matrix3x3 sqrtOfR;
    sqrtOfR << sqrt(__coilResistances(0,0)), 0, 0,
            0, sqrt(__coilResistances(1,1)), 0,
            0, 0, sqrt(__coilResistances(2,2));

    Math::Matrix3x3 returnMatrix( returnOrientation().inverse()* sqrtOfR );

    for( unsigned int i=0; i<3; i++ )
        for( unsigned int j=0; j<3; j++ )
            returnMatrix(i,j) = abs(returnMatrix(i,j));//Guarantee that resistances are positive!

    return returnMatrix;
}

Math::Vector Omnimagnet::getMaxMoment()
{
    Vector maxMoments;
    Vector maxCurrents(__p__CurrentOutputDevice->getCurrentLimits());

    //cout << endl << "Max Current: " << maxCurrents << endl;
    for( unsigned int i=0; i<3; i++) {
        maxMoments[i] = (1.0/__amp_per_dipoleMoment(i,i))*maxCurrents[i]; // calculate max moments
    }

    Matrix3x3 R(returnOrientation().inverse().asMatrix());// transpose it to convert from world to Omnimagnet to Omnimagnet to world conversion

    maxMoments = Vector(R*maxMoments); // rotate it to world frame;
    for( unsigned int i=0; i<3; i++) {
        maxMoments[i] = abs(maxMoments[i]); // make abs
    }

    //cout << maxMoments << endl;

    return maxMoments;
}

Math::Frame Omnimagnet::getFrame() const
{
    return Configuration6DOF_Producer::getFrame();
}
/*
void Omnimagnet::spinMoment() {
    dipoleMoment(__rotatingMoment);
}
bool Omnimagnet::startRotation() {
    // starts rotation
    __rotatingMoment.start();
    return startTicking<Omnimagnet>( *this, &Omnimagnet::spinMoment, 10);//update every 10ms
}

bool Omnimagnet::stopRotation() {
    // stops rotation
    __rotatingMoment.stop();
    return Timer::stopTicking();
}
bool Omnimagnet::rotationVector(const Vector& rotationVector) {
    // Length is rotation speed in rad/sec; direction is rotation axis
    __rotatingMoment.rotationAxis(rotationVector.normalized());
    __rotatingMoment.rotationSpeed(rotationVector.norm());
    return true;
}
Vector Omnimagnet::rotationVector( ) const {
    // returns the current rotation vector; length is speed, direction is rotation axis
    Vector returnVector = __rotatingMoment.rotationAxis();
    returnVector *= __rotatingMoment.rotationSpeed();
    return returnVector;
}
*/

// Force Producer Functions
bool Omnimagnet::forceAtPoint( const Math::ForceVector& force, const MagneticTool& endEffector)
{
    //Matrix3x3 Fmatrix;
    //Fmatrix = Magnetics::dipoleForceMatrix(endEffector.dipoleMoment,endEffector.position, this->returnPosition());

    Math::DipoleMomentVector dipMoment = Magnetics::dipoleMomentFromForce(force,endEffector.dipoleMoment,endEffector.position, this->returnPosition());

    return dipoleMoment( dipMoment );
}

ForceVector Omnimagnet::forceAtPoint( const MagneticTool& endEffector )
{

    return Magnetics::dipoleForce(endEffector.dipoleMoment,endEffector.position, returnPosition(), dipoleMoment());
}

// Torque Producer Functions
bool Omnimagnet::torqueAtPoint( const Math::TorqueVector& torque, const MagneticTool& endEffector)
{
    // T = mxB
    // for minimum B required
    // T_p = T - T dot (unitVec(m))*unitVec(m)
    // B = norm(T_p)/norm(m)*( unitVec(T_p) x unitVec(m) );

    // find torque orthogonal to dipole moment because no torque can be applied about the moment
    Vector torquePerp( torque.asVector(NEWTON_METER) - torque.asVector(NEWTON_METER).dot( endEffector.dipoleMoment.normalized().asVector() )*endEffector.dipoleMoment.normalized() );

    MagneticFieldVector B( torquePerp.norm()/endEffector.dipoleMoment.norm(AMP_METER_SQ) * torquePerp.normalized().cross(endEffector.dipoleMoment.normalized().asVector()), getFrame(), TESLA);
    return applyField( B , endEffector );
}

TorqueVector Omnimagnet::torqueAtPoint( const MagneticTool& endEffector )
{
    return Magnetics::dipoleTorque(endEffector.dipoleMoment, endEffector.position, returnPosition(), dipoleMoment());
}

bool Omnimagnet::fieldAtPoint( const Math::Direction& fieldDirection, const Position& field_position, DipoleMoment momentStrength )
{

    Math::Direction momentDirection =  Magnetics::dipoleMomentFromField( MagneticFieldVector(MagneticField(1.0),fieldDirection), field_position, returnPosition()).normalized();

    //Math::Direction fieldAtPoint((Math::Vector)(Magnetics::dipoleMatrix( field_position-position )*momentDirection));

    return dipoleMoment( DipoleMomentVector(momentStrength, momentDirection) );
}

bool Omnimagnet::applyField(const MagneticFieldVector& field, const Position& field_position)
{
    // commands a field value at a global location (Teslas and Meters)

    return dipoleMoment( Magnetics::dipoleMomentFromField(field, field_position, returnPosition()) );
}

bool Omnimagnet::deviceInAcceptableRange()
{
    // checks to see if all temperatures are below 100C and returns true if true and false if false

    Math::Vector temps = returnTemperature();

    if( temps.x_c() > MAX_TEMP )
        return false;
    if( temps.y_c() > MAX_TEMP )
        return false;
    if( temps.z_c() > MAX_TEMP )
        return false;

    return true; // if havet returned false yet it must be ok

}

void Omnimagnet::changeConfiguration6DOF_Producer( const Configuration6DOF_Producer& newProducer )
{
    this->Configuration6DOF_Producer::changeTo( newProducer );
}

/*
bool Omnimagnet::getPosition(Math::Position &position, distanceUnits units)
{
    position = returnPosition().convertToUnits(units);
    return true;
}


bool Omnimagnet::getOrientation(Math::Orientation &orientation) const
{
    orientation = this->Configuration6DOF::orientation();
    return true;
}

bool Omnimagnet::getConfiguration6DOF( Math::Configuration6DOF & configuration) const
{
    configuration = Configuration6DOF((*this));

    return true;
}

bool Omnimagnet::setPosition(const Math::Position &position)
{
    this->PositionProducer::_p_positionHistory->append(position) ;
    returnPosition() = position;
    return true;
}

bool Omnimagnet::setOrientation(const Math::Matrix3x3 & orientation )
{
    returnOrientation() = Orientation( orientation, returnPosition().getFrame(), OMNIMAGNET_FRAME);
    return true;
}

bool Omnimagnet::setConfiguration6Dof(const Configuration6DOF & configuration)
{
    setPosition(configuration.position());
    setOrientation(configuration.orientation().asMatrix());
    return true;
}
 */
