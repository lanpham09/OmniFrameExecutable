#include "Producers/Configuration5DOF_FromFile.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Utilities/Math.h"
#include "Utilities/Timer.h"

using namespace std;
using namespace Timing;
using namespace Math;

Configuration5DOF_FromFile::Configuration5DOF_FromFile( string fileName, double updateEvery_ms, bool loop,  string SignalName, distanceUnits  units, Frame frame, unsigned int historyLength ):
    Configuration5DOF_Producer(SignalName + " Position", units, frame, historyLength)
{
    this->thisType = TIMED;

    this->loop = loop;
    this->updateEvery_ms = updateEvery_ms;

    this->inputFile.open(fileName,ios_base::in);
    if( !inputFile.is_open() )
        cout << "Problem opening file: " + fileName + " \n";
    else
        this->fileName = fileName;

    _p_directionHistory = new AD_IO::analogVectorTimeStampedSignal(SignalName + " Direction","",500);

    goodValue = false;
}

Configuration5DOF_FromFile::Configuration5DOF_FromFile( string fileName, UPDATE_TYPE type, bool loop, string SignalName, distanceUnits  units, Frame frame, unsigned int historyLength):
    Configuration5DOF_Producer(SignalName + " Position", units, frame, historyLength)
{
    this->thisType = type;

    if( type == TIMED )
        cout << "Error in Configuration5DOF_FromFile Constructor:"
             << "\n\tFileName: " << fileName
             << "\n\tError: Cannot specify timed type with no time step use different constructor" << endl;

    this->loop = loop;
    this->updateEvery_ms = -1;

    this->inputFile.open(fileName,ios_base::in);
    if( !inputFile.is_open() )
        cout << "Problem opening file: " + fileName + " \n";
    else
        this->fileName = fileName;

    _p_directionHistory = new AD_IO::analogVectorTimeStampedSignal(SignalName + " Direction","",500);

    goodValue = false;

}


Configuration5DOF_FromFile::~Configuration5DOF_FromFile()
{
    this->stop();
    this->inputFile.close();

    delete _p_directionHistory;
}

bool Configuration5DOF_FromFile::start() {
    bool returnValue = false;

    if( this->inputFile.is_open() )
    {
        if( this->thisType == TIMED )
        {
            // Start Thread
            if( ! isTicking() )
            {
                cout << "START THREAD" << endl;
                returnValue= Timer::startTicking<Configuration5DOF_FromFile>( *this, &Configuration5DOF_FromFile::getNext, this->updateEvery_ms);//eg. update every 10ms

            }
            else {
                cout << "Error in Configuration5DOF_FromFile Start():"
                     << "\n\tFileName: " << fileName
                     << "\n\tError: Input already started.\n" << endl;
                returnValue= false;
            }

        }
    } else {
        cout << "Error in Configuration5DOF_FromFile Start():"
             << "\n\tFileName: " << fileName
             << "\n\tError: Input file not open.\n";
        returnValue = false;
    }

    return returnValue;
}
bool Configuration5DOF_FromFile::stop() {

    inputFile.close();

    return stopTicking();
}

bool Configuration5DOF_FromFile::getPosition(Position& position, distanceUnits units)
{
    if( (this->isTicking() || thisType != TIMED) && _p_positionHistory->historyLength() > 0)
    {
        if( thisType == EVERY_CALL )
            getNext();

        position = Position(_p_positionHistory->value().value, _frame, this->myPositionUnits ).convertToUnits(units);

        return goodValue;

    } else {
        position = Position(0,0,0,_frame,units);
        return false;
    }
}

bool Configuration5DOF_FromFile::getDirection(Direction& direction)
{

    if( (this->isTicking() || thisType != TIMED) && _p_directionHistory->historyLength() > 0)
    {
        if( thisType == EVERY_CALL )
            getNext();


        direction = Direction(_p_directionHistory->value().value, _frame);

        return goodValue;

    } else {
        direction = Direction(0,0,0,_frame);
        return false;
    }
}


bool Configuration5DOF_FromFile::getNext(double) {
    if( this->inputFile.eof() ) // if end of file
    {
        if( this->loop )
        {
            // need to loop over file again
            this->inputFile.close(); // close the file
            this->inputFile.open(this->fileName,ios_base::in);

        } else {
            this->stop();
        }
    }

    if( inputFile.good() ) {
        double x, y, z, xd, yd, zd; // xyz position vector, xd yd zd is direction vector

        if( inputFile.peek() ==',')
            inputFile.get();
        inputFile >> x;

        if( inputFile.peek() ==',')
            inputFile.get();
        inputFile >> y;

        if( inputFile.peek() ==',')
            inputFile.get();
        inputFile >> z;

        if( inputFile.peek() ==',')
            inputFile.get();
        inputFile >> xd;

        if( inputFile.peek() ==',')
            inputFile.get();
        inputFile >> yd;

        if( inputFile.peek() ==',')
            inputFile.get();
        inputFile >> zd;

        while( inputFile.peek() =='\n' || inputFile.peek() == '\r' )
            inputFile.get();

        _p_positionHistory->append( Vector(x,y,z) );
        _p_directionHistory->append( Direction(xd,yd,zd).asVector());

        goodValue = true;

    } else {
        // Something is wrong
        // no looping -> end of file means done or there is another problem
        goodValue = false;
        this->stop();
    }

    return goodValue;
}

bool Configuration5DOF_FromFile::fileOpen() const
{
    return inputFile.good();
}


AD_IO::analogVectorTimeStampedSignal* Configuration5DOF_FromFile::asDirectionHistoryPointer() const
{
    return _p_directionHistory;
}
