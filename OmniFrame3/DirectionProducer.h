/**
 * This is a virtual class that defines the basic functionality for classes that generate direction information
 * that will be consumed by other classes.  You need to implement all the virtual member functions even
 * if they aren't necessarily applicable to the functional class that you're implementing.
 *
 */

#ifndef _DIRECTION_PRODUCER_H_
#define _DIRECTION_PRODUCER_H_

#include "Utilities/Math.h"

class DirectionProducer {
public:
    DirectionProducer();
    DirectionProducer( const DirectionProducer & other );
    DirectionProducer( const Math::Direction& dir );
    virtual ~DirectionProducer();
    virtual bool getDirection( Math::Direction &direction );
    virtual Math::Direction returnDirection();
    virtual Math::Frame getFrame();

protected:
    virtual bool setDirection(Math::Direction &direction );
    virtual void set(double x, double y, double z);
    Math::Direction _direction;
};




#endif
