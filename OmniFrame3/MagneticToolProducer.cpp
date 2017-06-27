#include "Producers/MagneticToolProducer.h"

using namespace Math;


MagneticToolProducer::MagneticToolProducer(Position constPosition, DipoleMoment momentMagnitude, Direction constMomentDirection, ForceVector weight)
{
    assert(constPosition.getFrame() ==  constMomentDirection.getFrame() && constMomentDirection.getFrame() == weight.getFrame() );

    this->position = constPosition;
    this->moment = momentMagnitude;
    this->moment_direction = constMomentDirection;
    this->weight = weight;

    this->p_moment_directionProducer = 0;
    this->p_positionProducer = 0;
}

MagneticToolProducer::MagneticToolProducer(Position constPosition, DipoleMoment momentMagnitude, DirectionProducer* momentDirectionProducer, ForceVector weight)
{
    assert(constPosition.getFrame() ==  momentDirectionProducer->getFrame() && momentDirectionProducer->getFrame() == weight.getFrame() );

    this->position = constPosition;
    this->moment = momentMagnitude;
    this->moment_direction = Direction();
    this->weight = weight;

    this->p_moment_directionProducer = momentDirectionProducer;
    this->p_positionProducer = 0;
}

MagneticToolProducer::MagneticToolProducer(PositionProducer* positionProducer, DipoleMoment momentMagnitude, Direction constMomentDirection, ForceVector weight)
{
    assert(positionProducer->getFrame() ==  constMomentDirection.getFrame() && constMomentDirection.getFrame() == weight.getFrame() );

    this->position = Position();
    this->moment = momentMagnitude;
    this->moment_direction = constMomentDirection;
    this->weight = weight;

    this->p_moment_directionProducer = 0;
    this->p_positionProducer = positionProducer;
}

MagneticToolProducer::MagneticToolProducer(PositionProducer* positionProducer, DipoleMoment momentMagnitude, DirectionProducer* momentDirectionProducer, ForceVector weight)
{
    assert(positionProducer->getFrame() ==  momentDirectionProducer->getFrame() && momentDirectionProducer->getFrame() == weight.getFrame() );

    this->position = Position();
    this->moment = momentMagnitude;
    this->moment_direction = Direction();
    this->weight = weight;

    this->p_moment_directionProducer = momentDirectionProducer;
    this->p_positionProducer = positionProducer;
}

MagneticToolProducer::MagneticToolProducer(Position constPosition, DipoleMomentVector dipoleMomentVector, ForceVector weight)
{
    assert(constPosition.getFrame() ==  dipoleMomentVector.getFrame() && dipoleMomentVector.getFrame() == weight.getFrame() );

    this->position = constPosition;
    this->moment = dipoleMomentVector.norm();
    this->moment_direction = dipoleMomentVector.normalized();
    this->weight = weight;

    this->p_moment_directionProducer = 0;
    this->p_positionProducer = 0;
}

MagneticToolProducer::MagneticToolProducer(PositionProducer* positionProducer, DipoleMomentVector dipoleMomentVector, ForceVector weight)
{
    assert(positionProducer->getFrame() ==  dipoleMomentVector.getFrame() && dipoleMomentVector.getFrame() == weight.getFrame() );

    this->position = Position();
    this->moment = dipoleMomentVector.norm();
    this->moment_direction = dipoleMomentVector.normalized();
    this->weight = weight;

    this->p_moment_directionProducer = 0;
    this->p_positionProducer = positionProducer;
}

void MagneticToolProducer::setWeight( const ForceVector& weight )
{
    assert( this->getFrame() == weight.getFrame() );
    this->weight = weight;
}

void MagneticToolProducer::setPosition( const Position& position )
{
    p_positionProducer = 0;
    this->position = position;
}

void MagneticToolProducer::setDirection( const Direction& moment_direction )
{
    p_moment_directionProducer = 0;
    this->moment_direction = moment_direction;
}

void MagneticToolProducer::setDirection( DirectionProducer* moment_directionProducer)
{
    p_moment_directionProducer = moment_directionProducer;
}

void MagneticToolProducer::setPosition( PositionProducer* positionProducer )
{
    p_positionProducer = positionProducer;
}





void MagneticToolProducer::setDipoleStrength( const DipoleMoment& strength)
{
    this->moment = strength;
}

void MagneticToolProducer::setDipoleMoment( const DipoleMomentVector& newMoment)
{
    setDirection(newMoment.normalized());
    setDipoleStrength(newMoment.norm());
}

bool MagneticToolProducer::getMagneticTool(Math::MagneticTool& tool)
{
    bool retVal = true;
    if( p_moment_directionProducer )
        retVal &= p_moment_directionProducer->getDirection(moment_direction);

    if( p_positionProducer )
        retVal &= p_positionProducer->getPosition(position);

    tool = MagneticTool( position, moment_direction*moment, weight );

    return retVal;
}

MagneticTool MagneticToolProducer::returnMagneticTool()
{
    MagneticTool tmp;
    getMagneticTool(tmp);

    return tmp;
}

Frame MagneticToolProducer::getFrame()
{

    if( p_positionProducer )
        return p_positionProducer->getFrame();
    else
        return position.getFrame();

}
