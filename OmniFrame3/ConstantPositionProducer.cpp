#include "ConstantPositionProducer.h"

ConstantPositionProducer::ConstantPositionProducer(Math::Position pos, unsigned int historyLength, std::string name):
    PositionProducer(name , pos.getUnits(), pos.getFrame(), historyLength )
{
    setPosition(pos);

}

bool ConstantPositionProducer::getPosition(Math::Position &position, distanceUnits units)
{
    if( units == DIST_NATIVE )
        units = myPositionUnits;

    position = Math::Position(this->_p_positionHistory->value(units).value, _frame, units); //this->_position;
    return true;
}

void ConstantPositionProducer::setPosition(const Math::Position& pos)
{
     _p_positionHistory->append( pos );//this->_position = pos;
    // myPositionUnits.changeBaseUnits(pos.getUnits());
     changeFrame( pos.getFrame() );
}
