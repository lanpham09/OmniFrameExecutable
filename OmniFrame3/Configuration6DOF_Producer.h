#ifndef CONFIGURATION6DOF_PRODUCER_H
#define CONFIGURATION6DOF_PRODUCER_H

#include "Producers/PositionProducer.h"
#include "Producers/OrientationProducer.h"
#include "Utilities/Math.h"
#include <string>

class Configuration6DOF_Producer:public PositionProducer, public OrientationProducer
{
protected:
    Math::Position _constPosition;

    PositionProducer* p_positionProducer;
    OrientationProducer* p_orientationProducer;

    Math::HomogeneousTransform _transform;
    bool isFromHomogeniusTransform;


    void changeTo( const Configuration6DOF_Producer& other );

public:
    Configuration6DOF_Producer( std::string name = "Default 6DOF Configuration Producer", distanceUnits units = METER, Math::Frame from_frame = Math::UNDEFINED_FRAME, Math::Frame to_frame = Math::UNDEFINED_FRAME, unsigned int historyLength = 1 );
    Configuration6DOF_Producer( const Math::Configuration6DOF& , std::string name = "Constant Position Producer" );
    Configuration6DOF_Producer( const Math::Position&,  const Math::Orientation&, std::string name = "Constant Position Producer");
    Configuration6DOF_Producer( PositionProducer*,      const Math::Orientation& );
    Configuration6DOF_Producer( const Math::Position&,  OrientationProducer* orientProducer, std::string name = "Constant Position Producer");
    Configuration6DOF_Producer( PositionProducer*,      OrientationProducer* orientProducer );
    Configuration6DOF_Producer( const Math::HomogeneousTransform& transform );
    Configuration6DOF_Producer( const Configuration6DOF_Producer & other );


    virtual bool getConfiguration6DOF(Math::Configuration6DOF& pose);
    virtual Math::Configuration6DOF returnConfiguration6DOF();

    virtual bool getPosition(Math::Position &position, distanceUnits units = DIST_NATIVE);
    virtual Math::Position  returnPosition();
    virtual AD_IO::distanceSensorVector* asPositionHistoryPointer() const;
    virtual std::string positionProducerName() const;
    virtual distanceUnits getUnits() const;

    virtual bool getOrientation(Math::Orientation& orient);
    virtual Math::Orientation returnOrientation();

    virtual Math::Frame getFrame() const;
};

#endif // CONFIGURATION6DOF_PRODUCER_H
