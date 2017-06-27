#include "Producers/DirectionProducer.h"

DirectionProducer::DirectionProducer()
{
    _direction = Math::Direction();
}

DirectionProducer::DirectionProducer(const DirectionProducer & other)
{
    _direction = other._direction;
}

DirectionProducer::DirectionProducer( const Math::Direction& dir )
{
    _direction = dir;
}

bool DirectionProducer::getDirection(Math::Direction &direction)
{
    direction = _direction;
    return true;
}

Math::Direction DirectionProducer::returnDirection()
{
    Math::Direction temp;
    getDirection(temp);
    return temp;
}

Math::Frame DirectionProducer::getFrame()
{
    return returnDirection().getFrame();
}

bool DirectionProducer::setDirection(Math::Direction &direction)
{
    _direction = direction;
    return true;
}

void DirectionProducer::set(double x, double y, double z)
{
    _direction.set(x,y,z);
}

DirectionProducer::~DirectionProducer()
{
    return;
}
