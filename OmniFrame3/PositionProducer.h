/**
 * This is a virtual class that defines the basic functionality for classes that generate position information
 * that will be consumed by other classes.  You need to implement all the virtual member functions even
 * if they aren't necessarily applicable to the functional class that you're implementing.
 *
 */

#ifndef _POSITION_PRODUCER_H_
#define _POSITION_PRODUCER_H_

#include "Utilities/Math.h"

#include "Utilities/Units.h"
#include "AD_IO/SensorTypes.h"
#include <string>


class PositionProducer {
protected:
    AD_IO::distanceSensorVector *_p_positionHistory;
    distanceUnits myPositionUnits;
    void changeFrame( const Math::Frame & newFrame );

    Math::Frame _frame;
    //Math::Position _position;

public:
    PositionProducer();
    PositionProducer(std::string name, const distanceUnits & dUnits, const Math::Frame & frame, unsigned int historyLength = 1);
    PositionProducer( const PositionProducer& other );
    PositionProducer(const Math::Position& constPosition );

    virtual ~PositionProducer();

    virtual bool getPosition(Math::Position &position, distanceUnits units = DIST_NATIVE);
    virtual Math::Position  returnPosition();
    PositionProducer* asPositionProducer();
    virtual AD_IO::distanceSensorVector* asPositionHistoryPointer() const;

    virtual string positionProducerName() const;
    virtual distanceUnits getUnits() const;

    virtual Math::Frame getFrame() const;

};




#endif
