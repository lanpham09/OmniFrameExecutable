/*   vectorFromFile.h (requires vectorFromFile.cpp)
        Reads in a comma seporated text file of vector information x, y, z at a specified rate.
        Acts like a position producer or direction producer.


Author: Andrew Petruska
Date Created: 9-4-2012
Date Modified: 9-4-2012
*/

#ifndef CONFIGURATION5DOF_FROM_FILE_H
#define CONFIGURATION5DOF_FROM_FILE_H

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "AD_IO/AD_IO.h"
#include "Producers/PositionProducer.h" // inherited Type
#include "Producers/DirectionProducer.h" // inherited Type
#include "Producers/Configuration5DOF_Producer.h"
#include "Utilities/Units.h"

#include "Utilities/Math.h" // Defines Position and Direction
using namespace Math;

#include "Utilities/Timer.h"  // controls loop callback
using namespace Timing;


class Configuration5DOF_FromFile: public Configuration5DOF_Producer, private Timer {
public:
    enum UPDATE_TYPE { EVERY_CALL, MANUAL, TIMED };

    Configuration5DOF_FromFile( string fileName, double updateEvery_ms, bool loop = false, string SignalName = "Unnamed Configuration5DOF From File",  distanceUnits  units = METER, Frame frame = UNDEFINED_FRAME, unsigned int historyLength = 1);
    Configuration5DOF_FromFile( string fileName, UPDATE_TYPE type = MANUAL, bool loop = false, string SignalName = "Unnamed Configuration5DOF From File", distanceUnits units = METER, Frame frame = UNDEFINED_FRAME, unsigned int historyLength = 1);
    ~Configuration5DOF_FromFile();

    bool getNext(double=0);

    bool start();
    bool stop();

    bool fileOpen() const;

    bool getPosition( Position& position, distanceUnits units = DIST_NATIVE);
    bool getDirection( Direction& direction );

    AD_IO::analogVectorTimeStampedSignal* asDirectionHistoryPointer() const;

private:

    UPDATE_TYPE thisType;

    Math::Vector lastReadLine;

    fstream inputFile;
    string fileName;

    double updateEvery_ms;
    bool loop;
    bool goodValue;

    AD_IO::analogVectorTimeStampedSignal* _p_directionHistory;


    // Cannot Set Positions this is a file input!
    void setConfiguration5DOF(const Math::Configuration5DOF& ){return;}
    void setPosition(const Math::Position &){return;}
    void setPosition(PositionProducer*){return;}
    void setDirection(const Math::Direction &){return;}
    void setDirection(DirectionProducer*){return;}
};


#endif
