#include "Producers/TransformedConfiguration5DOF_Producer.h"

#include <iostream>
#include <fstream>
#include "Producers/PositionProducer.h"
#include "Utilities/Math.h"

using namespace std;
using namespace Math;


/**
 * This constrcutor takes as input the name of the text file where the coordinate transformation matrix is stored
 * (see the TransformedConfiguration5DOF_producer.h for details), and the input and output coordinate frames for this
 * transformation.
 *
 */
TransformedConfiguration5DOF_producer::TransformedConfiguration5DOF_producer(string fileName, Configuration5DOF_Producer *input_producer, unsigned int historyLength ):
    Configuration5DOF_Producer(input_producer->asPositionHistoryPointer()->name() + " in ", input_producer->getUnits(), UNDEFINED_FRAME, historyLength)
{

    initialized = false;

    if (!input_producer) {
        cout << "TransformedConfiguration5DOF_producer: a input PositionProducer has not been provided." << endl;
        return;
    }

    this->input_producer = input_producer;

    initialized = transformation.readTransformFromFile( fileName );
    this->changeFrame(transformation.toFrame());
    this->asPositionHistoryPointer()->name( input_producer->asPositionHistoryPointer()->name() + " in " + transformation.toFrame().frameName() );

    if( initialized )
        cout << ("TransformedConfiguration5DOF_producer: matrix transform from " + transformation.fromFrame().frameName() + " to " + transformation.toFrame().frameName() + " has been successfuly read. To convert Producer: " + input_producer->positionProducerName() +"\n" );
    else
        cout << "TransformedConfiguration5DOF_producer: Could not read transformation file to convert Producer: " + input_producer->positionProducerName() +"\n";

    assert( input_producer->getFrame() == transformation.fromFrame() && initialized);

    return;
}

TransformedConfiguration5DOF_producer::TransformedConfiguration5DOF_producer( const Math::HomogeneousTransform& transformation, Configuration5DOF_Producer *input_producer, unsigned int historyLength ):
    Configuration5DOF_Producer(input_producer->asPositionHistoryPointer()->name() + "in " + (string)transformation.toFrame(), input_producer->getUnits(), transformation.toFrame(), historyLength )
{
    this->transformation = transformation;
    this->input_producer = input_producer;

    if( this->input_producer->getFrame() != transformation.fromFrame() )
    {
        cout << "ERROR: Homogeneous transformation will be invalid because transfromation From frame ( " + transformation.fromFrame().frameName() + " ) is different than " +input_producer->positionProducerName()  + "'s frame ( " + input_producer->getFrame().frameName() +" )\n";
        assert(input_producer->getFrame() == transformation.fromFrame());
    }

    initialized = true;

    return;
}

TransformedConfiguration5DOF_producer::TransformedConfiguration5DOF_producer( const Math::HomogeneousTransform& transformation, const Configuration5DOF_Producer& producerToCopy ):
    Configuration5DOF_Producer(producerToCopy)
{
    this->transformation = transformation;
    input_producer = this;
    this->initialized = true;
    assert( transformation.fromFrame() == producerToCopy.getFrame());

}

/**
 * Places the transformed position into output_position, return true upon success. An
 * error has occurred if this function returns false.
 *
 */
bool TransformedConfiguration5DOF_producer::getPosition(Math::Position &output_position, distanceUnits units) {


    if (!initialized)
        return false;

    Math::Position input_position;

    bool goodPosition = input_producer->getPosition(input_position);

    if ( !goodPosition )
    {
        cout << "TRANSFORMATION: could not get an input position!" << endl;
    }


    output_position = (transformation * input_position).convertToUnits( units );

    //cout << input_position << "\t" << output_position <<endl;


    _p_positionHistory->appendIfChanged( output_position );


    return (goodPosition && initialized);
}

bool TransformedConfiguration5DOF_producer::getDirection(Math::Direction& dir)
{
    if (!initialized)
        return false;

    Math::Direction input_dir;

    bool goodDirection = input_producer->getDirection(input_dir);
    if (!goodDirection)
    {
        cout << "TransformedConfiguration5DOF_producer: could not get an input direction!" << endl;
    }


    dir = (transformation * input_dir);

    return initialized && goodDirection;
}

bool TransformedConfiguration5DOF_producer::getConfiguration5DOF(Math::Configuration5DOF &pose)
{
    if (!initialized)
        return false;

    Math::Configuration5DOF input_pose;

    bool goodPose = input_producer->getConfiguration5DOF(input_pose);

    if (!goodPose)
    {
        cout << "TransformedConfiguration5DOF_producer: could not get an input 5 DOF pose!" << endl;
    }

    pose = (transformation * input_pose);

    return initialized && goodPose;
}


void TransformedConfiguration5DOF_producer::changeProducer( Configuration5DOF_Producer* newProducer )
{
    input_producer = newProducer;
    assert( input_producer->Configuration5DOF_Producer::getFrame() == transformation.fromFrame() );
}

void TransformedConfiguration5DOF_producer::changeTransform( const Math::HomogeneousTransform& newTransform )
{
    transformation = newTransform;
    assert( input_producer->Configuration5DOF_Producer::getFrame() == transformation.fromFrame() );
}

void TransformedConfiguration5DOF_producer::changeTransform( const string& newFileName )
{
    initialized = transformation.readTransformFromFile(newFileName);

    assert( input_producer->Configuration5DOF_Producer::getFrame() == transformation.fromFrame() );
}

Math::HomogeneousTransform  TransformedConfiguration5DOF_producer::getTransformation() const
{
    return transformation;
}

Math::Frame TransformedConfiguration5DOF_producer::getFrame() const
{
    return transformation.toFrame();
}
