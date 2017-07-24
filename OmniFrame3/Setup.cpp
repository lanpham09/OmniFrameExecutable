#include "Setup.h"
#include <QDebug>

Setup::Setup():
    systemSetup( "Omnimagnet System Data Log Files", true )
{
    //**********************
    //  CONTROLLER
    //**********************

    // SETUP CONTROLLER
    //cout << "SETUP POSITION CONTROLLER" << endl;
    //std::vector<double> Numerator = {  0.3, - 0.2675};
    //std::vector<double> Denominator = {1, - 0.3509};

    // Amdrew's PD controller: need PD controller for "Find Tool Weight"
    // comes from Kp = 0.0002, Kd = 0.05, pole = 125
    std::vector<double> Numerator = {  0.62, -0.6009};
    std::vector<double> Denominator = {1,    -0.04321};

    positionController = new DiscreteVectorController(Numerator,Denominator);

    // Setup Force Signal
    desiredForce = new analogVectorTimeStampedSignal("desired Force","N",500);

    //********************************
    //  INDIVIDUAL OMNIMAGNET SETUP
    //********************************
        //ArduinoSetup class checks if Due's are connected to the computer and has them start sending data. The connect command than takes the sensor data and
        //translate it to xyz cooridates with respect to the world axis at the center of the table. The coordinates are than updated to the omnimagnets. This
        //makes the program automatically adjust to the number of Arms sending data.


    //Setup on the arduinos
    Arduinos = new ArduinoSetup;
    //Using the arduino sensor data update the configuration of each connected omnimagnet
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateConfiguration()));
    updateTimer.start( 100 );
    cout << "Arduinos connected \n";


    //**********************************************
    //  TOOL AND TRAJECTORY SETUP FOR DEMO
    //**********************************************

    // ***Setup Tool Location. Currently adjusted for a static tool position instead of the
    //  one found from the camera. TransformationProducer is used with cameras instead of constant ************Change tool location back to 0,0,0
    //***tooLocation also translate to actual position
    toolLocation = new ConstantPositionProducer( Math::Position(10,10,10, WORLD_FRAME, MILLIMETER));

    //original:  toolLocation = new TransformedPositionProducer( CameraToWorld , stereoCamera->asPositionProducer(),500);

    //Original Line of Code, has been moved to after we know how many magnets are connected, currently unsure if location of this line matters/how it effect the program
    // ***Setup Goal Pose Info!!!does this still work without the big magnet and random configuration?
    //goodDirection = new TwoOmnimagnet_SingularDirectionProducer(systemSetup(OmniMag0),systemSetup(OmniMag2),toolLocation,TwoOmnimagnet_SingularDirectionProducer::WELL_CONDITIONED);


    // ***SETUP Trajectory
    // manual selection of trajectory configuration, this data is only used when the demo is running
    //trajectory_PositionProducer = new Configuration5DOF_FromFile("cube.csv", 1000.0/20.0, true, "Trajectory", METER, TRAJECTORY_FRAME, 500);
    //**goal position translates to desired position and direction gives heading
    //trajectory_PositionProducer = new Configuration5DOF_FromFile("stationary.csv", Configuration5DOF_FromFile::MANUAL, true, "Trajectory", METER, TRAJECTORY_FRAME, 500);
    //trajectory_PositionProducer->getNext(); // Get the first position
    trajectory_PositionProducer = new Configuration5DOF_Producer(Math::Position(10,10,10,TRAJECTORY_FRAME, MILLIMETER), Math::Direction(1,1,1,TRAJECTORY_FRAME));

    Eigen::Matrix3d val;    //No change in orientation from trajectory frame to world frame
    val <<  1, 0, 0,
            0, 1, 0,
            0, 0, 1;
    //A homogeneous tranform from the Trajectory Frame to World Frame
    TrajectoryToWorld = Math::HomogeneousTransform(val, Math::Displacement(20, 20, 20, Math::WORLD_FRAME,MILLIMETER), TRAJECTORY_FRAME, WORLD_FRAME);

    goalPose = new TransformedConfiguration5DOF_producer(TrajectoryToWorld ,trajectory_PositionProducer);
    //goalLocation is currently not used, it's only purpose seems to be to change the goal pose later
    //goalPose = new  Configuration5DOF_Producer(goalLocation, goalDirection);
    goalLocation = goalPose ->asPositionProducer();

    // ***SETUP MAGNETIC TOOL INFO
    //The dipole moment is calculated by the magnet’s Volume * remanent Magnetization (in Tesla) / mu_0.
    DipoleMoment toolDipoleMoment(.126);  // Magnetic Tool info for Andrew's nice pill
    //DipoleMoment toolDipoleMoment(0);     // Nonmagnetic Tool

    ForceVector toolWeight(0,0,-.001230, WORLD_FRAME, NEWTON); // Effective Weight given boyancy.  Can change in time from water obsorption need to modify during runs
    //magnet tool includes information regarding the trajectory and goal position
    toolProducer = new MagneticToolProducer( toolLocation, toolDipoleMoment, goalPose, toolWeight);
    magneticTool = new SinglePerminantMagnetTool(toolProducer);

    //************************************
    // MULTIPLE OMNIMAGNET SETUP
    //************************************
    //Setup/Determine how many omnimagnets are active (OmniFrame Arm is sending data) and add them to the MultipleOmnimagetSystem
    //Omnimagents can be manually added to the system by using pushback and setup a fix configuration for the magnet commented out below else
    //an omnimagnet without a working due will not be initalized and added to the system.
    //Requires tool setup to be completed

    std::vector<Omnimagnet*> omnimagnetPointerList;     //vector of pointer to active omnimagnets
    for (int i = 0; i <= Arduinos->number_of_arduinos; ++i){
        if (Arduinos->arduino_id[i] == 0) {
            omnimagnetPointerList.push_back(systemSetup(OmniMag1));
        }
        if (Arduinos->arduino_id[i] == 1) {
            omnimagnetPointerList.push_back(systemSetup(OmniMag2));
        }
        if (Arduinos->arduino_id[i] == 2) {
            omnimagnetPointerList.push_back(systemSetup(OmniMag3));
        }
        if (Arduinos->arduino_id[i] == 3) {
            omnimagnetPointerList.push_back(systemSetup(OmniMag4));
        }
        if (Arduinos->arduino_id[i] == 4) {
            omnimagnetPointerList.push_back(systemSetup(OmniMag5));
        }
        if (Arduinos->arduino_id[i] == 5) {
            omnimagnetPointerList.push_back(systemSetup(OmniMag6));
        }
    }

        //******************************
        // MANUAL SETUP OF OMNIMAGNETS
        //******************************
        /*
        //Manually configure an omnimagnet
        //Math::Matrix3x3 orient;
        //orient << 1 0 0
        //          0 1 0
        //          0 0 1
        //Math::Orientation OmnimagnetOrientation = Math::Orientation(orient, WORLD_FRAME, OMNIMAGNET_FRAME);
        //Math::Postion OmnimagnetPosition = Math::Position(x, y ,z , WORLD_FRAME,OMNIMAGNET_FRAME);
        //
        //default/manual omnimaget setup. This manual setups up omnimagets 1, 2 ,3 ,4 and 5.
        //std::vector< Omnimagnet* > omnimagnetPointerList = {systemSetup(OmniMag0), systemSetup(OmniMag2), systemSetup(OmniMag3), systemSetup(OmniMag4), systemSetup(OmniMag5)};
        //maually add magnet
        //omnimagnetPointerList.push_back(systemSetup(OmniMag#));
        //
        */


    // ***Strictly for the 2 magnet case, it determines which direction maximizes force control capabilities
    if (omnimagnetPointerList.size() == 2){

        goodDirection = new TwoOmnimagnet_SingularDirectionProducer(omnimagnetPointerList[0],omnimagnetPointerList[1],toolLocation,TwoOmnimagnet_SingularDirectionProducer::WELL_CONDITIONED);
        cout << "2 Omnimagnet optimize direction for force control completed";
    }

    //setup the system of multiple omnimagnets: (list of pointers, magnet tool configuration, optimization settting, frame of reference)
    omnimagSystem = new MultipleOmnimagnetSystem(omnimagnetPointerList, magneticTool, MultipleOmnimagnetSystem::MINIMIZE_POWER, Math::WORLD_FRAME);

    cout << "Omnimagnet System Setup and Enabled. "<< omnimagnetPointerList.size() << " Omnimagent(s) Active in System. \n";

    //***********************************
    //  FILE LOG SETUP
    //***********************************

{
    // setup file logging
    fileLog = systemSetup.dataLogger;

    // add video Write Signal
//    fileLog->addSignal(videoWriteSignal);
//    fileLog->addSignal(leftPixelLocation);
//    fileLog->addSignal(rightPixelLocation);
//    fileLog->addSignal(goal_LeftPixelLocation);
//    fileLog->addSignal(goal_RightPixelLocation);

    // add desired trajectory signal
    fileLog->addSignal( trajectory_PositionProducer->asPositionHistoryPointer() );
    fileLog->addSignal( goalPose->asPositionHistoryPointer() );
    //fileLog->addSignal( trajectory_PositionProducer->asDirectionHistoryPointer());
    fileLog->addSignal( desiredForce );

    // add tool location
    fileLog->addSignal( toolLocation->asPositionHistoryPointer() );

    // add system inhibit signal
    fileLog->addSignal(systemSetup.inhibitSignalPointer());
    fileLog->addSignal(omnimagSystem->p_powerSignal);

    // Initialize Zero Time
    fileLog ->clearHistory();
    fileLog ->resetTimeStamp();
    fileLog ->writeAllEvery( 250 );


    cout << "Writing to file log.\n";
    }
}

void Setup::updateConfiguration(){
    //updates the position configuration using the sensor readings from the Due.

    Math::Orientation OmnimagnetOrientation;
    Math::Position OmnimagnetPosition;
    double omni_orient;
    int arduino_id;
    double basetoworld_offset_x[6] = {0 , 0, 0, 0, 0, 0};
    double basetoworld_offset_y[6] = {0 , 0, 0, 0, 0, 0};


    for (int i = 0; i <= Arduinos->number_of_arduinos; ++i ){

        arduino_id = Arduinos->arduino_id[i];
        double pi = 3.14159;
        double post = Arduinos->OmniFrame[arduino_id][0]*pi/180;
        double hor = Arduinos->OmniFrame[arduino_id][1];
        double ver = Arduinos->OmniFrame[arduino_id][2];
        double omni = Arduinos->OmniFrame[arduino_id][3]*pi/180;

        if (arduino_id <= 3 ){
            omni_orient = pi/180*(post+omni);
        }
        else{
            omni_orient = pi*180*(-(post+omni));
        }
        //CALCULATING ORIENTATION
        Math::Matrix3x3 orient;
        //orient matrices are calculated correctly
        orient<< cos(omni_orient), -sin(omni_orient)*cos(omni_orient), sin(omni_orient)*sin(omni_orient),
                sin(omni_orient), cos(omni_orient)*cos(omni_orient) , -cos(omni_orient)*sin(omni_orient),
                0            , sin(omni_orient)               , cos(omni_orient);

        OmnimagnetOrientation = Math::Orientation( orient, WORLD_FRAME, OMNIMAGNET_FRAME);

        //***Must perform tranformation from base to world frame at center
        double position[3];
        position[0] = cos(post)*hor + basetoworld_offset_x[arduino_id] ;
        position[1] = sin(post)*hor + basetoworld_offset_y[arduino_id] ;
        position[2] = ver;

        //adjust position from base of arm to world
        OmnimagnetPosition = Math::Position( position[0], position[1], position[2], WORLD_FRAME, MILLIMETER );

        if (arduino_id == 0 ){
            systemSetup(OmniMag1)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));}
        if (arduino_id == 1 ){
            systemSetup(OmniMag2)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));}
        if (arduino_id == 2 ){
            systemSetup(OmniMag3)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));}
        if (arduino_id == 3 ){
            systemSetup(OmniMag4)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));}
        if (arduino_id == 4 ){
            systemSetup(OmniMag5)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));}
        if (arduino_id == 5 ){
            systemSetup(OmniMag6)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));}

    }
}

Setup::~Setup()
{
    systemSetup.inhibit();

    delete positionController;
    delete desiredForce;
    delete toolLocation;
    delete goodDirection;
    delete trajectory_PositionProducer;
    delete goalPose;
    delete toolProducer;
    delete magneticTool;
    delete omnimagSystem;
}


void Setup::OFF()
{
    omnimagSystem->OFF();
}


