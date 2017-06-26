#include "OrientationProducer.h"

OrientationProducer::OrientationProducer()
{
    _orientation = Math::Orientation();
}

OrientationProducer::OrientationProducer( const Math::Orientation& constOrientation )
{
    _orientation = constOrientation;
}

OrientationProducer::OrientationProducer( const OrientationProducer& other )
{
    _orientation = other._orientation;
}

OrientationProducer & OrientationProducer::operator = (const OrientationProducer& other)
{
     _orientation = other._orientation;
     return *this;
}

bool OrientationProducer::getOrientation(Math::Orientation& orient)
{
    orient = _orientation;
    return true;
}

Math::Orientation OrientationProducer::returnOrientation()
{
    getOrientation( _orientation );
    return _orientation;
}

Math::Frame OrientationProducer::getFrame()
{
    return returnOrientation().getBaseFrame();
}

Math::Frame OrientationProducer::getEndFrame()
{
    return returnOrientation().getMyFrame();
}

