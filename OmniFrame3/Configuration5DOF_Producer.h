#ifndef CONFIGURATION5DOF_PRODUCER_H
#define CONFIGURATION5DOF_PRODUCER_H


#include "Producers/PositionProducer.h"
#include "Producers/DirectionProducer.h"
#include "Utilities/Math.h"
#include <string>

class Configuration5DOF_Producer:public PositionProducer, public DirectionProducer
{
protected:
    Math::Position _constPosition;

    PositionProducer* p_positionProducer;
    DirectionProducer* p_DirectionProducer;

public:
    Configuration5DOF_Producer( std::string name = "Default 6DOF Configuration Producer", distanceUnits units = METER, Math::Frame frame = Math::UNDEFINED_FRAME, unsigned int historyLength = 1 );
    Configuration5DOF_Producer( const Math::Configuration5DOF& , std::string name = "Constant Configuration 5DOF Producer" );
    Configuration5DOF_Producer( const Math::Position&,  const Math::Direction&, std::string name = "Constant Position Producer");
    Configuration5DOF_Producer( PositionProducer*,      const Math::Direction& );
    Configuration5DOF_Producer( const Math::Position&,  DirectionProducer* dirProducer, std::string name = "Constant Position Producer");
    Configuration5DOF_Producer( PositionProducer*,      DirectionProducer* dirProducer );
    Configuration5DOF_Producer( const Configuration5DOF_Producer& other );

    virtual bool getConfiguration5DOF(Math::Configuration5DOF& pose);
    Math::Configuration5DOF returnConfiguration5DOF();
    virtual void setConfiguration5DOF(const Math::Configuration5DOF& );

    virtual bool getPosition(Math::Position &position, distanceUnits units = DIST_NATIVE);
    virtual void setPosition(const Math::Position &);
    virtual void setPosition(PositionProducer*);

    virtual AD_IO::distanceSensorVector* asPositionHistoryPointer() const;
    virtual std::string positionProducerName() const;
    virtual distanceUnits getUnits() const;

    virtual bool getDirection(Math::Direction& dir);
    virtual void setDirection(const Math::Direction &);
    virtual void setDirection(DirectionProducer*);

    virtual Math::Frame getFrame() const;
};

#endif // CONFIGURATION5DOF_PRODUCER_H
