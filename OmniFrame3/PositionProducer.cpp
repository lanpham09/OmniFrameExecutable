#include "PositionProducer.h"
#include "Utilities/Math.h"

PositionProducer::PositionProducer()
{
    _p_positionHistory = new AD_IO::distanceSensorVector(1.0,Math::Zero(),"Base Position Producer", METER, 1);
    myPositionUnits = METER;
    _frame = Math::UNDEFINED_FRAME;
    return;
}

PositionProducer::PositionProducer(std::string name, const distanceUnits & dUnits, const Math::Frame & frame, unsigned int historyLength )
{

    _p_positionHistory = new AD_IO::distanceSensorVector(1.0,Math::Zero(), name, dUnits, historyLength);
    myPositionUnits = dUnits;
    _frame = frame;
    return;
}

PositionProducer::PositionProducer( const PositionProducer& other )
{
    myPositionUnits = other.myPositionUnits;
    _frame = other._frame;

    _p_positionHistory = new AD_IO::distanceSensorVector( *(other._p_positionHistory) );
}

PositionProducer::PositionProducer(const Math::Position& constPosition )
{
    myPositionUnits = constPosition.getUnits();
    _frame = constPosition.getFrame();

    _p_positionHistory = new AD_IO::distanceSensorVector(1.0,Math::Zero(),"Const Position Producer", myPositionUnits, 100);
    _p_positionHistory->append(constPosition);
}

PositionProducer::~PositionProducer()
{
    delete _p_positionHistory;
}

string PositionProducer::positionProducerName() const
{
    return _p_positionHistory->name();
}


bool PositionProducer::getPosition(Math::Position &position, distanceUnits units )
{
    if ( units == distanceUnits::NATIVE )
        units = myPositionUnits;

    if( this->_p_positionHistory->historyLength() > 0 )
    {
        position =  Math::Position(this->_p_positionHistory->value(units).value, _frame, units);
        return true;
    } else
    {
        position =  Math::Position(0, 0, 0, _frame, units);
        return false;
    }

    return true;
}

Math::Position PositionProducer::returnPosition()
{
    Math::Position temp;
    getPosition(temp);
    return temp;
}

PositionProducer* PositionProducer::asPositionProducer()
{
    return this;
}

AD_IO::distanceSensorVector* PositionProducer::asPositionHistoryPointer() const
{
    return _p_positionHistory;
}

distanceUnits PositionProducer::getUnits() const
{
    return this->myPositionUnits;
}

void PositionProducer::changeFrame( const Math::Frame & newFrame )
{
    _frame = newFrame;
}

Math::Frame PositionProducer::getFrame() const
{
    return _frame;
}

