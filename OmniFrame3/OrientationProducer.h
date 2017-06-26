#ifndef ORIENTATIONPRODUCER_H
#define ORIENTATIONPRODUCER_H

#include "Utilities/Math.h"

class OrientationProducer
{
protected:
    Math::Orientation _orientation;

public:
    OrientationProducer();
    OrientationProducer( const Math::Orientation& constOrientation );
    OrientationProducer( const OrientationProducer& other);

    OrientationProducer & operator = (const OrientationProducer& other);

    virtual bool getOrientation(Math::Orientation& orient);
    virtual Math::Orientation returnOrientation();
    virtual Math::Frame getFrame();
    virtual Math::Frame getEndFrame();

};

#endif // ORIENTATIONPRODUCER_H
