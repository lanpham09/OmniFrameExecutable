#include "TwoOmnimagnet_SingularDirectionProducer.h"


TwoOmnimagnet_SingularDirectionProducer::TwoOmnimagnet_SingularDirectionProducer( PositionProducer* OmniOne_PositionProducer, PositionProducer* OmniTwo_PositionProducer, PositionProducer* Tool_PositionProducer , mode thisMode )
{
    this->_mode = thisMode;

    this->OmniOne_PositionProducer = OmniOne_PositionProducer;
    this->OmniTwo_PositionProducer = OmniTwo_PositionProducer;
    this->Tool_PositionProducer = Tool_PositionProducer;

    this->initialized = false;
    //this->e1_last = Vector(0.0,0.0,0.0);


}

bool TwoOmnimagnet_SingularDirectionProducer::getDirection(Direction &direction) {

    Position ToolPos = Tool_PositionProducer->returnPosition();
    Position OmniOnePos = OmniOne_PositionProducer->returnPosition();
    Position OmniTwoPos = OmniTwo_PositionProducer->returnPosition();

    Vector p1 = ( ToolPos - OmniOnePos ).asVector(METER);
    Vector p2 = ( ToolPos - OmniTwoPos ).asVector(METER);

    // Ouf of Plane Singular Direction
    Vector e1( p1.cross(p2) );
    Frame workingFrame = ToolPos.getFrame();

    if( e1.norm() == 0 )
    {
        // in line singularity
        Vector x(1,0,0);
        e1 = Vector( p1.cross(x) );
        if ( e1.norm() == 0 ) {
            Vector y(0,1,0);
            e1 = Vector( p1.cross(y) );
        }
    }

    if (initialized)
    {
        //if ( sign(e1.dot(Vector(1.0,0.0,0.0))) != sign(e1_last.dot(Vector(1.0,0.0,0.0))) )
        //    e1 = Vector(-e1.x() , -e1.y() , -e1.z() );

        e1 = sign(e1.dot(e1_last))*e1;
    }
    e1_last = e1;

    // In Plane Singular Direction
    Vector e2 ( e1.cross( p1*pow(p2.norm(),2.0) - p2*pow(p1.norm(),2.0) ));     // not dividing by denominator, cause direction
    if (initialized) {
       // if ( sign(e2.dot(Vector(1.0,0.0,0.0))) != sign(e2_last.dot(Vector(1.0,0.0,0.0))) )
       //     e2 = Vector(-e2.x() , -e2.y() , -e2.z() );
        e2 = sign(e2.dot(e2_last))*e2;
    }
    e2_last = e2;

    // Well Conditioned Direction
    Vector e3 ( p1*pow(p2.norm(),2.0)*(-1) + p2*pow(p1.norm(),2.0) ) ;     // not dividing by denominator, cause direction
    if (initialized) {
        //if ( sign(e3.dot(Vector(1.0,0.0,0.0))) != sign(e3_last.dot(Vector(1.0,0.0,0.0))) )
        //    e3 = Vector(-e3.x() , -e3.y() , -e3.z() );
        e3 = sign(e3.dot(e3_last))*e3;

    }
    e3_last = e3;

    initialized = true;


    // return the appropriate vector
    if ( _mode == IN_PLANE )
        direction = Direction(e2,workingFrame);
    else if (_mode == OUT_OF_PLANE)
        direction = Direction(e1,workingFrame);
    else if (_mode == WELL_CONDITIONED)
        direction = Direction(e3,workingFrame);
    else
        return false;

    return true;
}


Math::Frame TwoOmnimagnet_SingularDirectionProducer::getFrame()
{
    return OmniOne_PositionProducer->getFrame();
}
