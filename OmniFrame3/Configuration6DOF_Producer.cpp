#include "Producers/Configuration6DOF_Producer.h"


Configuration6DOF_Producer::Configuration6DOF_Producer( std::string name, distanceUnits units, Math::Frame from_frame, Math::Frame to_frame, unsigned int historyLength ):
    PositionProducer(name, units, to_frame, historyLength),
    OrientationProducer(Math::Orientation(Math::Identity(), from_frame, to_frame )),
    isFromHomogeniusTransform(false)
{
    _constPosition = Math::Position(Math::Zero(),to_frame,units);

    p_positionProducer = 0;
    p_orientationProducer = 0;
}

Configuration6DOF_Producer::Configuration6DOF_Producer( const Math::Configuration6DOF& config, std::string name):
    PositionProducer(name,config.getUnits(),config.getFrame(),1),
    OrientationProducer(config.orientation()),
    isFromHomogeniusTransform(false)
{
    PositionProducer::asPositionHistoryPointer()->append(config.position().asVector());

    _constPosition = config.position();

    assert( _constPosition.getFrame() == _orientation.getBaseFrame() );

    p_positionProducer = 0;
    p_orientationProducer = 0;
}

Configuration6DOF_Producer::Configuration6DOF_Producer( const Math::Position& pos,  const Math::Orientation& orient, std::string name):
    PositionProducer(name,pos.getUnits(),pos.getFrame(),1),
    OrientationProducer(orient),
    isFromHomogeniusTransform(false)
{
    PositionProducer::asPositionHistoryPointer()->append(pos.asVector());

    _constPosition = pos;

    assert( _constPosition.getFrame() == _orientation.getBaseFrame() );

    p_positionProducer = 0;
    p_orientationProducer = 0;
}

Configuration6DOF_Producer::Configuration6DOF_Producer( PositionProducer* pProducer,      const Math::Orientation& orient ):
    OrientationProducer(orient),
    isFromHomogeniusTransform(false)
{
    _constPosition = Math::Position();

    p_positionProducer = pProducer;
    p_orientationProducer = 0;

    assert( pProducer->getFrame() == _orientation.getBaseFrame() );
}

Configuration6DOF_Producer::Configuration6DOF_Producer( const Math::Position& pos,  OrientationProducer* pOrient, std::string name):
    PositionProducer(name,pos.getUnits(),pos.getFrame(),1),
    OrientationProducer(),
    isFromHomogeniusTransform(false)
{
    PositionProducer::asPositionHistoryPointer()->append(pos.asVector());

    _constPosition = pos;

    p_positionProducer = 0;
    p_orientationProducer = pOrient;

    assert( _constPosition.getFrame() == p_orientationProducer->getFrame() );
}

Configuration6DOF_Producer::Configuration6DOF_Producer( PositionProducer* pProducer,      OrientationProducer* oProducer ):
    isFromHomogeniusTransform(false)
{
    _constPosition = Math::Position();

    p_positionProducer = pProducer;
    p_orientationProducer = oProducer;

    assert( p_positionProducer->getFrame() == p_orientationProducer->getFrame() );

}

Configuration6DOF_Producer::Configuration6DOF_Producer(  const Configuration6DOF_Producer& other  ):
    PositionProducer(other),
    OrientationProducer( other ),
    _constPosition( other._constPosition ),
    p_positionProducer(other.p_positionProducer),
    p_orientationProducer(other.p_orientationProducer),
    _transform(other._transform),
    isFromHomogeniusTransform(other.isFromHomogeniusTransform)
{
    return ;
}

Configuration6DOF_Producer::Configuration6DOF_Producer(  const Math::HomogeneousTransform& transform  ):
    PositionProducer(transform.toFrame().frameName() + " Configuration", transform.getTranslation().getUnits(), transform.toFrame(), 200),
    OrientationProducer( transform.toOrientation() ),
    isFromHomogeniusTransform(false)
{
    _constPosition = Math::Position();

    p_positionProducer = 0;
    p_orientationProducer = 0;
}

bool Configuration6DOF_Producer::getConfiguration6DOF(Math::Configuration6DOF& pose)
{
    bool retValue = true;

    if( p_positionProducer != 0 )
        retValue &= p_positionProducer->getPosition(_constPosition);
    if( p_orientationProducer != 0 )
        retValue &= p_orientationProducer->getOrientation(_orientation);

    if(  isFromHomogeniusTransform )
    {
        pose = _transform.toConfiguration6DOF();

    } else
    {
        pose = Math::Configuration6DOF( _constPosition, _orientation );

    }
    return retValue;
}

Math::Configuration6DOF Configuration6DOF_Producer::returnConfiguration6DOF()
{
    Math::Configuration6DOF temp;
    getConfiguration6DOF(temp);

    return temp;
}


Math::Frame Configuration6DOF_Producer::getFrame() const
{
    Math::Frame pFrame = _constPosition.getFrame();
    Math::Frame oFrame = _orientation.getBaseFrame();

    if( p_positionProducer != 0 )
        pFrame = p_positionProducer->getFrame();
    if( p_orientationProducer != 0 )
        oFrame = p_orientationProducer->getFrame();

    if(  isFromHomogeniusTransform )
        oFrame = pFrame = _transform.toFrame();

    assert( pFrame == oFrame );

    return pFrame;
}

bool Configuration6DOF_Producer::getPosition(Math::Position &position, distanceUnits units)
{
    bool retValue = true;

    if( p_positionProducer )
        retValue &= p_positionProducer->getPosition(_constPosition);

    if( isFromHomogeniusTransform )
    {
        _constPosition = _transform.toPosition();
        _p_positionHistory->appendIfChanged( _constPosition );
    }

    position = _constPosition;
    position.convertToUnits( units );

    return retValue;
}

Math::Position  Configuration6DOF_Producer::returnPosition()
{
    Math::Position retPos(_constPosition);
    getPosition(retPos,this->getUnits());

    return retPos;
}

AD_IO::distanceSensorVector* Configuration6DOF_Producer::asPositionHistoryPointer() const
{
    if( p_positionProducer != 0 )
        return p_positionProducer->asPositionHistoryPointer();

    return PositionProducer::asPositionHistoryPointer();
}
string Configuration6DOF_Producer::positionProducerName() const
{
    if( p_positionProducer != 0 )
        return p_positionProducer->positionProducerName();

    return PositionProducer::positionProducerName();
}

distanceUnits Configuration6DOF_Producer::getUnits() const
{
    if( p_positionProducer != 0 )
        return p_positionProducer->getUnits();
    else if( isFromHomogeniusTransform )
        return _transform.getTranslation().getUnits();

    return PositionProducer::getUnits();
}

bool Configuration6DOF_Producer::getOrientation(Math::Orientation& orient)
{

    bool retValue = true;

    if( p_orientationProducer )
    {
        retValue &= p_orientationProducer->getOrientation(_orientation);
    }
    else if( isFromHomogeniusTransform )
    {
        _orientation = _transform.toOrientation();
    }

    orient = _orientation;

    return retValue;
}

Math::Orientation Configuration6DOF_Producer::returnOrientation()
{
    Math::Orientation orient(_orientation);
    getOrientation(orient);

    return orient;
}


void Configuration6DOF_Producer::changeTo( const Configuration6DOF_Producer& other )
{
    _orientation              = other._orientation;
    _constPosition            = other._constPosition;
    p_positionProducer        = other.p_positionProducer;
    p_orientationProducer     = other.p_orientationProducer;
    isFromHomogeniusTransform = other.isFromHomogeniusTransform;
    _transform                = other._transform;


    if( !p_positionProducer && !isFromHomogeniusTransform )
        _p_positionHistory->append( _constPosition );
}
