#ifndef CONSTANTPOSITIONPRODUCER_H
#define CONSTANTPOSITIONPRODUCER_H

#include "Producers/PositionProducer.h"
#include "Utilities/Math.h"

#include <string>

class ConstantPositionProducer:public PositionProducer
{

public:
    ConstantPositionProducer(Math::Position pos, unsigned int historyLength = 1, std::string name ="Constant Position");
    bool getPosition(Math::Position &position, distanceUnits units = DIST_NATIVE);
    void setPosition(const Math::Position& pos);
};

#endif // CONSTANTPOSITIONPRODUCER_H
