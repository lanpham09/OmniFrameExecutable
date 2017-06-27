#ifndef TRANSFORMEDCONFIGURATION5DOF_PRODUCER_H
#define TRANSFORMEDCONFIGURATION5DOF_PRODUCER_H


#include "Producers/Configuration5DOF_Producer.h"
#include "Utilities/Math.h"
#include <string>

class TransformedConfiguration5DOF_producer : public Configuration5DOF_Producer {
    private:
        bool initialized;

        Configuration5DOF_Producer *input_producer;

        Math::HomogeneousTransform transformation;

    public:


        TransformedConfiguration5DOF_producer( string fileName, Configuration5DOF_Producer *input_producer, unsigned int historyLength = 1 );
        TransformedConfiguration5DOF_producer( const Math::HomogeneousTransform& transformation, Configuration5DOF_Producer* , unsigned int historyLength = 1 );
        TransformedConfiguration5DOF_producer( const Math::HomogeneousTransform& transformation, const Configuration5DOF_Producer& producerToCopy );

        void changeProducer( Configuration5DOF_Producer* newProducer );
        void changeTransform( const Math::HomogeneousTransform& newTransform );
        void changeTransform( const string& newFileName );

        virtual bool getPosition(Math::Position &output_position, distanceUnits units = DIST_NATIVE);

        virtual bool getDirection(Math::Direction& dir);

        virtual bool getConfiguration5DOF(Math::Configuration5DOF &pose);

        Math::HomogeneousTransform  getTransformation() const;

        virtual Math::Frame getFrame() const;

};


#endif // TRANSFORMEDCONFIGURATION5DOF_PRODUCER_H
