#include "Setup.h"
#include <QDebug>
///
/// \brief Setup::Setup
///
/// Setup of OmniFrame System:
///
///     1. The controller setup was developed by Andrew to find the correct tool weight when manipulating an object in the fish tank.
///     2. Individual Omnimagnet setup works by checking to see how many arduinos are connected to the computer and updating the Omnimagnet
///         configuration accordingly. Currently all connected Arduinos are added to the Omnimagnet System but this may change depending on
///         how many magnets the user requires
///     3. Tool and trajectory setup is important for when the user wants to implement a tool following a trajectory. Even if this isn't the goal
///         many of the components of the setup are required for the Omnimagnet System Setup. It is important to adjust the tool magnetic dipole
///         accordingly for the tool or to set to zero for no tool ( may implement a setting for no tool (button) ).
///     4. Computer vision setup to use the PtGrey cameras for tool tracking. Computer vision tracking is required for any trajectory implementation
///         in order to prevent the system from going unstable (maybe Katies work could be used to make the system not require computer vision)
///     5. Omnimagnet System Setup currently determines how many magnets are connected and adds them to a pointer list of omnimagnets. There is also
///         an option to manually active omnimagnets with fixed positions.
///     6. File log is suppose to log the information to a folder but currently don't know where its saving the data
///
///
Setup::Setup():
    systemSetup( "Omnimagnet System Data Log Files", true )
{
//    //initalizing variable
    for (int i = 0; i < 6; ++i){
        omni_orient[i] = 0;
    }

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
    updateTimer.start( 100 );   //updates every 100 milliseconds
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
    //Andrew's setup:  DipoleMoment toolDipoleMoment(.126);  // Magnetic Tool info for Andrew's nice pill

    //Andrew's setup:  ForceVector toolWeight(0,0,-.001230, WORLD_FRAME, NEWTON); // Effective Weight given boyancy.  Can change in time from water obsorption need to modify during runs
    //magnet tool includes information regarding the trajectory and goal position

    DipoleMoment toolDipoleMoment(10);   //no magnet/nonmagnet tool
    ForceVector toolWeight(0,0,10,WORLD_FRAME,NEWTON);    //No tool thus no weight
    toolProducer = new MagneticToolProducer( toolLocation, toolDipoleMoment, goalPose, toolWeight);
    magneticTool = new SinglePerminantMagnetTool(toolProducer);

    //************************************
    // MULTIPLE OMNIMAGNET SETUP
    //************************************
    //Setup/Determine how many omnimagnets are active (OmniFrame Arm is sending data) and add them to the MultipleOmnimagetSystem
    //Omnimagents can be manually added to the system by using pushback and setup a fix configuration for the magnet commented out below else
    //an omnimagnet without a working due will not be initalized and added to the system.
    //Requires tool setup to be completed

    //This doesn't work because the  number of arduinos connected is not determined until after everything has been executed and things are then "connected"
    std::vector<Omnimagnet*> omnimagnetPointerList;     //vector of pointer to active omnimagnets
//    for (int i = 0; i <= Arduinos->number_of_arduinos; ++i){
//        if (Arduinos->arduino_id[i] == 0) {
//            omnimagnetPointerList.push_back(systemSetup(OmniMag1));
//        }
//        if (Arduinos->arduino_id[i] == 1) {
//            omnimagnetPointerList.push_back(systemSetup(OmniMag2));
//        }
//        if (Arduinos->arduino_id[i] == 2) {
//            omnimagnetPointerList.push_back(systemSetup(OmniMag3));
//        }
//        if (Arduinos->arduino_id[i] == 3) {
//            omnimagnetPointerList.push_back(systemSetup(OmniMag4));
//        }
//        if (Arduinos->arduino_id[i] == 4) {
//            omnimagnetPointerList.push_back(systemSetup(OmniMag5));
//        }
//        if (Arduinos->arduino_id[i] == 5) {
//            omnimagnetPointerList.push_back(systemSetup(OmniMag6));
//        }
//    }

    //******************************
    // MANUAL SETUP OF OMNIMAGNETS
    //******************************

    //Manually configure an omnimagnet
//    Math::Matrix3x3 orient;
//    orient << 1, 0, 0,
//            0, 1, 0,
//            0, 0, 1;
//    Math::Orientation OmnimagnetOrientation = Math::Orientation(orient, WORLD_FRAME, OMNIMAGNET_FRAME);
//    Math::Position OmnimagnetPosition = Math::Position(135, 0 ,84 , WORLD_FRAME, MILLIMETER);
//    systemSetup(OmniMag4)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));

//    orient << 1, 0, 0,
//            0, 1, 0,
//            0, 0, 1;
//    OmnimagnetOrientation = Math::Orientation(orient, WORLD_FRAME, OMNIMAGNET_FRAME);
//    OmnimagnetPosition = Math::Position(-56, 0 ,214 , WORLD_FRAME, MILLIMETER);
//    systemSetup(OmniMag5)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));

    //default/manual omnimaget setup. This manual setups up omnimagets 1, 2 ,3 ,4 and 5.
    //std::vector< Omnimagnet* > omnimagnetPointerList = {systemSetup(OmniMag1)};
    //maually add magnet
    //omnimagnetPointerList.push_back(systemSetup(OmniMag4));
    //omnimagnetPointerList.push_back(systemSetup(OmniMag5));




    // ***Strictly for the 2 magnet case, it determines which direction maximizes force control capabilities
    //    if (omnimagnetPointerList.size() == 2){

    //        goodDirection = new TwoOmnimagnet_SingularDirectionProducer(omnimagnetPointerList[0],omnimagnetPointerList[1],toolLocation,TwoOmnimagnet_SingularDirectionProducer::WELL_CONDITIONED);
    //        cout << "2 Omnimagnet optimize direction for force control completed";
    //    }

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

//*****************
//  FUNCTIONS
//*****************

void Setup::updateConfiguration(){
    ///Updates the position configuration using the sensor readings from the Due. Converts from Omnimagnet Frame to World Frame (@ center of table)

    Math::Orientation OmnimagnetOrientation;
    Math::Position OmnimagnetPosition;
    int arduino_id;
    double basetoworld_xoffset[6] = {486,0,-486,486,0,-486};    //offsets used to transform Omni Frame to World Frame
    double basetoworld_yoffset[6] = {-283,-283,-283,283,283,283};
    double orient_offset[6] = {90,-90,90,270,270,270};   //offset in orientation because each omnimagnit fits into the arm at a diff orientation
    double position[3] = {0,0,0};

    for (int i = 0; i <= Arduinos->number_of_arduinos; ++i ){

        //assign arduino readings to corresponding variables
        arduino_id = Arduinos->arduino_id[i];
        double pi = 3.14159;
        double post = Arduinos->OmniFrame[arduino_id][0]*pi/180;
        double hor = Arduinos->OmniFrame[arduino_id][1];
        double ver = Arduinos->OmniFrame[arduino_id][2];
        double omni = Arduinos->OmniFrame[arduino_id][3]*pi/180;

        //Omni Frames are offset by 90 deg CW from the world frame
        omni_orient[arduino_id] = post+omni+orient_offset[arduino_id]*pi/180;

        //CALCULATING ORIENTATION
        Math::Matrix3x3 orient;

        //Omni Orientation revolves around the z axis
        orient<< cos(omni_orient[arduino_id]), -sin(omni_orient[arduino_id]), 0,
                sin(omni_orient[arduino_id]), cos(omni_orient[arduino_id]) , 0,
                0               , 0                , 1;

        OmnimagnetOrientation = Math::Orientation( orient, WORLD_FRAME, OMNIMAGNET_FRAME);

        //Omni Frame is oriented the same as World Frame
        if (arduino_id < 3){
            //***Must perform tranformation from base to world frame at center
            position[0] = cos(post)*hor + cos(post - 90*pi/180)*69.0 + basetoworld_xoffset[arduino_id]; //x: accounting for rotating about post
            position[1] = sin(post)*hor + sin(post - 90*pi/180)*69.0 + basetoworld_yoffset[arduino_id]; //y: accounting for rotating about post
            position[2] = ver;  //z: arduino script already accounts for beam deflection

        }
        //OmniFrame is the reverse of the World Frame
        else {
            position[0] = -cos(post)*hor - cos(post - 90*pi/180)*69.0 + basetoworld_xoffset[arduino_id]; //x: accounting for rotating about post
            position[1] = -sin(post)*hor - sin(post - 90*pi/180)*69.0 + basetoworld_yoffset[arduino_id]; //y: accounting for rotating about post
            position[2] = ver;  //z: arduino script already accounts for beam deflection

        }
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


