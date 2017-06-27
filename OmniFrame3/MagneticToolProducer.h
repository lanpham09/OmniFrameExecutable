/**
 * This is a virtual class that defines the basic functionality for classes that generates Magnetic Tool information
 * that will be consumed by other classes.  You need to implement all the virtual member functions even
 * if they aren't necessarily applicable to the functional class that you're implementing.
 *
 */

#ifndef _MAGNETIC_TOOL_PRODUCER_H_
#define _MAGNETIC_TOOL_PRODUCER_H_

#include "Utilities/Math.h"
#include "Producers/DirectionProducer.h"
#include "Producers/PositionProducer.h"

using namespace Math;
class MagneticToolProducer {
private:
    DipoleMoment moment;

    ForceVector weight;

    Direction moment_direction;

    Position position;

    DirectionProducer* p_moment_directionProducer;
    PositionProducer*  p_positionProducer;

public:

    MagneticToolProducer(Position constPosition, DipoleMoment momentMagnitude, Direction constMomentDirection, ForceVector weight);
    MagneticToolProducer(Position constPosition, DipoleMoment momentMagnitude, DirectionProducer* momentDirectionProducer, ForceVector weight);
    MagneticToolProducer(PositionProducer* positionProducer, DipoleMoment momentMagnitude, Direction constMomentDirection, ForceVector weight);
    MagneticToolProducer(PositionProducer* positionProducer, DipoleMoment momentMagnitude, DirectionProducer* momentDirectionProducer, ForceVector weight);
    MagneticToolProducer(Position constPosition, DipoleMomentVector dipoleMomentVector, ForceVector weight);
    MagneticToolProducer(PositionProducer* positionProducer, DipoleMomentVector dipoleMomentVector, ForceVector weight);

    virtual void setWeight( const ForceVector& weight );
    virtual void setDipoleStrength( const DipoleMoment& strength);
    virtual void setDipoleMoment( const DipoleMomentVector& newMoment);
    virtual void setPosition( const Position& position );
    virtual void setPosition( PositionProducer* positionProducer );
    virtual void setDirection( const Direction& dir );
    virtual void setDirection( DirectionProducer* moment_directionProducer);

    virtual bool getMagneticTool(Math::MagneticTool& tool);
    virtual Math::MagneticTool returnMagneticTool();
    virtual Math::Frame getFrame();
};




#endif
