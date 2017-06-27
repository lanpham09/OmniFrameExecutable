#include "Producers/Configuration5DOF_Producer.h"

Configuration5DOF_Producer::Configuration5DOF_Producer( std::string name, distanceUnits units, Math::Frame in_frame, unsigned int historyLength ):
    PositionProducer(name, units, in_frame, historyLength)
{
    _constPosition = Math::Position(Math::Zero(),in_frame,units);
    _direction = Math::Direction(0,0,1, in_frame );
    p_positionProducer = 0;
    p_DirectionProducer = 0;

    this->_p_positionHistory->appendIfChanged(_constPosition);
}

Configuration5DOF_Producer::Configuration5DOF_Producer( const Math::Configuration5DOF& config, std::string name):
    PositionProducer(name,config.getUnits(),config.getFrame(),1)
{
    PositionProducer::asPositionHistoryPointer()->append(config.position().asVector());

    _constPosition = config.position();
    _direction = config.direction();

    assert( _constPosition.getFrame() == _direction.getFrame() );

    p_positionProducer = 0;
    p_DirectionProducer = 0;

    this->_p_positionHistory->appendIfChanged(_constPosition);
}

Configuration5DOF_Producer::Configuration5DOF_Producer( const Math::Position& pos,  const Math::Direction& dir, std::string name):
    PositionProducer(name,pos.getUnits(),pos.getFrame(),1)
{
    PositionProducer::asPositionHistoryPointer()->append(pos.asVector());

    _constPosition = pos;
    _direction = dir;

    assert( _constPosition.getFrame() == _direction.getFrame() );

    p_positionProducer = 0;
    p_DirectionProducer = 0;

    this->_p_positionHistory->appendIfChanged(_constPosition);
}

Configuration5DOF_Producer::Configuration5DOF_Producer( PositionProducer* pProducer,      const Math::Direction& dir )
{
    _constPosition = Math::Position();
    _direction = dir;

    p_positionProducer = pProducer;
    p_DirectionProducer = 0;

    assert( pProducer->getFrame() == _direction.getFrame() );

    this->_p_positionHistory->appendIfChanged(_constPosition);
}

Configuration5DOF_Producer::Configuration5DOF_Producer( const Math::Position& pos,  DirectionProducer* pdir, std::string name):
    PositionProducer(name,pos.getUnits(),pos.getFrame(),1)
{
    PositionProducer::asPositionHistoryPointer()->append(pos.asVector());

    _constPosition = pos;
    _direction = Math::Direction();

    p_positionProducer = 0;
    p_DirectionProducer = pdir;

    assert( _constPosition.getFrame() == p_DirectionProducer->getFrame() );

    this->_p_positionHistory->appendIfChanged(_constPosition);
}

Configuration5DOF_Producer::Configuration5DOF_Producer( PositionProducer* pProducer,      DirectionProducer* oProducer )
{
    _constPosition = Math::Position();
    _direction = Math::Direction();

    p_positionProducer = pProducer;
    p_DirectionProducer = oProducer;

    assert( p_positionProducer->getFrame() == p_DirectionProducer->getFrame() );

    this->_p_positionHistory->appendIfChanged(_constPosition);

}

Configuration5DOF_Producer::Configuration5DOF_Producer( const Configuration5DOF_Producer& other ):
    PositionProducer(other), DirectionProducer(other)
{
    _constPosition = other._constPosition;

    p_positionProducer = other.p_positionProducer;
    p_DirectionProducer = other.p_DirectionProducer;

    this->_p_positionHistory->appendIfChanged(_constPosition);

}

bool Configuration5DOF_Producer::getConfiguration5DOF(Math::Configuration5DOF& pose)
{
    bool retValue = true;

    if( p_positionProducer != 0 )
        retValue &= p_positionProducer->getPosition(_constPosition);
    if( p_DirectionProducer != 0 )
        retValue &= p_DirectionProducer->getDirection(_direction);

    pose = Math::Configuration5DOF( _constPosition, _direction );

    this->_p_positionHistory->appendIfChanged(_constPosition);

    return retValue;

}

Math::Configuration5DOF Configuration5DOF_Producer::returnConfiguration5DOF()
{
    Math::Configuration5DOF temp;
    getConfiguration5DOF(temp);
    return temp;
}

Math::Frame Configuration5DOF_Producer::getFrame() const
{
    Math::Frame pFrame = _constPosition.getFrame();
    Math::Frame dFrame = _direction.getFrame();

    if( p_positionProducer != 0 )
        pFrame = p_positionProducer->getFrame();
    if( p_DirectionProducer != 0 )
        dFrame = p_DirectionProducer->getFrame();

    assert( pFrame == dFrame );

    return pFrame;
}

bool Configuration5DOF_Producer::getPosition(Math::Position &position, distanceUnits units)
{


    bool retValue = true;
    if( p_positionProducer != 0 )
        retValue &= p_positionProducer->getPosition(_constPosition);

    this->_p_positionHistory->appendIfChanged(_constPosition);

    position = _constPosition;
    position.convertToUnits( units );

    return retValue;
}

AD_IO::distanceSensorVector* Configuration5DOF_Producer::asPositionHistoryPointer() const
{
    if( p_positionProducer != 0 )
        return p_positionProducer->asPositionHistoryPointer();

    return PositionProducer::asPositionHistoryPointer();
}
string Configuration5DOF_Producer::positionProducerName() const
{
    if( p_positionProducer != 0 )
        return p_positionProducer->positionProducerName();

    return PositionProducer::positionProducerName();
}

distanceUnits Configuration5DOF_Producer::getUnits() const
{
    if( p_positionProducer != 0 )
        return p_positionProducer->getUnits();

    return PositionProducer::getUnits();
}

bool Configuration5DOF_Producer::getDirection(Math::Direction& dir)
{

    bool retValue = true;

    if( p_DirectionProducer != 0 )
        retValue &= p_DirectionProducer->getDirection(_direction);

    dir = _direction;

    return retValue;
}


void Configuration5DOF_Producer::setPosition(const Math::Position & newPos)
{
    assert( newPos.getFrame() ==  getFrame() );
    _constPosition = newPos;
    p_positionProducer = 0;

    this->_p_positionHistory->appendIfChanged(_constPosition);
}

void Configuration5DOF_Producer::setPosition(PositionProducer* newPosProducer)
{
    assert( newPosProducer->getFrame() == getFrame() );
    p_positionProducer = newPosProducer;
}

void Configuration5DOF_Producer::setDirection(const Math::Direction & newDir)
{
    assert( newDir.getFrame() == getFrame() );
    _direction = newDir;
    p_DirectionProducer = 0;
}

void Configuration5DOF_Producer::setDirection(DirectionProducer* newDirProducer)
{
    assert( newDirProducer->getFrame() == getFrame() );
    p_DirectionProducer = newDirProducer;
}

void Configuration5DOF_Producer::setConfiguration5DOF(const Math::Configuration5DOF& other)
{
    _constPosition = other.position();
    _direction = other.direction();
    p_positionProducer = 0;
    p_DirectionProducer = 0;

    this->_p_positionHistory->appendIfChanged(_constPosition);
}

