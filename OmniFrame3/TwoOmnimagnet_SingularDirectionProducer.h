#ifndef TWOOMNIMAGNET_SINGULARDIRECTIONPRODUCER_H
#define TWOOMNIMAGNET_SINGULARDIRECTIONPRODUCER_H

#include "Producers/PositionProducer.h"
#include "Producers/DirectionProducer.h"
#include "Utilities/Math.h" // Defines Position and Direction
using namespace Math;



class TwoOmnimagnet_SingularDirectionProducer : public DirectionProducer
{

public:
    enum mode {IN_PLANE, OUT_OF_PLANE, WELL_CONDITIONED};
    TwoOmnimagnet_SingularDirectionProducer( PositionProducer* OmniOne_PositionProducer, PositionProducer* OmniTwo_PositionProducer, PositionProducer* Tool_PositionProducer , mode thisMode );

//    Direction OutOfPlaneSingularDirection() const;
//    Direction InPlaneSingularDirection() const;
//    Direction WellConditionedDirection() const;

    bool getDirection(Direction &direction);
    Math::Frame getFrame();

private:

    mode _mode;
    PositionProducer* OmniOne_PositionProducer;
    PositionProducer* OmniTwo_PositionProducer;
    PositionProducer* Tool_PositionProducer;

    Math::Vector e1_last;
    Math::Vector e2_last;
    Math::Vector e3_last;
    bool initialized;

};

#endif // TWOOMNIMAGNET_SINGULARDIRECTIONPRODUCER_H
