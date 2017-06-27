#include "Setup.h"

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

    Matrix3x3 orient;
    Math::Position OmnimagnetPosition;
    Math::Orientation OmnimagnetOrientation;

    //*******SET UP OMNIMAGNET 0***********
    // Omimagnet 0 Setup (Big Omnimag) LP: Only last configuration is used since others are overwritten
    OmnimagnetPosition = Math::Position( 85, 155, 330, WORLD_FRAME, MILLIMETER );  // can adjust configuration

    orient <<   0,  0,   1,
                1,  0,   0,
                0,  1,   0;
    OmnimagnetOrientation = Math::Orientation( orient, WORLD_FRAME, OMNIMAGNET_FRAME);
    // Create Omnimagnet
    //Uses the MultipleOmnimagnetSetup class which configures all of the Omnimagnets. This line access the first Omnimagnet and
    //changes it's configuration from referencing itself as zero to the position/orientation specified.
    systemSetup(OmniMag0)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));


    //********SETUP OMNIMAGNET 2************
    // Omimagnet 2 Setup (Small Omnimagnet)
    // Arduino Position Input Orientation
    orient << 	 1,  0,  0,
                 0,  1,  0,
                 0,  0,  1;
    //omniMag2Location = new TransformedPositionProducer( HomogeneousTransform(orient, Math::Displacement(275,453,120,Math::WORLD_FRAME,MILLIMETER),omni2Pos->getFrame(), Math::WORLD_FRAME), omni2Pos, 500);
    OmnimagnetPosition = Math::Position( -95.0, 40.0, 65, WORLD_FRAME, MILLIMETER );  // Config F5 F4 F4b F3 F5b


    // Omnimagent Orientation (From World Frame to Omnimagnet Frame)
    orient << 	 1,  0,  0,
                 0,  1,  0,
                 0,  0,  1;
    OmnimagnetOrientation = Math::Orientation( orient, WORLD_FRAME, OMNIMAGNET_FRAME);
    systemSetup(OmniMag2)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));


    //*********SETUP OMNIMAGNET 3****************
    // Omimagnet 3 Setup (Small Omnimagnet)
    OmnimagnetPosition = Math::Position( 260, 330, 65, WORLD_FRAME, MILLIMETER );  // Config F5b

    // Omnimagent Orientation (From World Frame to Omnimagnet Frame)
    orient << 	 1,  0,  0,
                 0,  1,  0,
                 0,  0,  1;
    OmnimagnetOrientation = Math::Orientation( orient, WORLD_FRAME, OMNIMAGNET_FRAME);
    systemSetup(OmniMag3)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));

    //************SETUP OMNIMAGNET 4************
    // Omimagnet 4 Setup (Small Omnimagnet)
    OmnimagnetPosition = Math::Position( 260, 330, 190, WORLD_FRAME, MILLIMETER );  // Config F5b

    // Omnimagent Orientation (From World Frame to Omnimagnet Frame)
    orient << 	 1,  0,  0,
                 0,  1,  0,
                 0,  0,  1;
    OmnimagnetOrientation = Math::Orientation( orient, WORLD_FRAME, OMNIMAGNET_FRAME);
    systemSetup(OmniMag4)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));


    //*************SETUP OMNIMAGNET 5*******************
    // Omimagnet 5 Setup (Small Omnimagnet)
    OmnimagnetPosition = Math::Position( 255.0, 50, 65, WORLD_FRAME, MILLIMETER );

    // Omnimagent Orientation (From World Frame to Omnimagnet Frame)
    orient << 	 1,  0,  0,
                 0,  1,  0,
                 0,  0,  1;
    OmnimagnetOrientation = Math::Orientation( orient, WORLD_FRAME, OMNIMAGNET_FRAME);
    systemSetup(OmniMag5)->changeConfiguration6DOF_Producer( Configuration6DOF_Producer( OmnimagnetPosition, OmnimagnetOrientation ));

    //**********************************************
    //  TOOL AND TRAJECTORY SETUP
    //**********************************************

    // ***Setup Tool Location. Currently adjusted for a static tool position instead of the
    //  one found from the camera. TransformationProducer is used with cameras instead of constant
    toolLocation = new ConstantPositionProducer( Math::Position(0,0,0, WORLD_FRAME, MILLIMETER));

    // ***Setup Goal Pose Info
    goodDirection = new TwoOmnimagnet_SingularDirectionProducer(systemSetup(OmniMag0),systemSetup(OmniMag2),toolLocation,TwoOmnimagnet_SingularDirectionProducer::WELL_CONDITIONED);

    // ***SETUP Trajectory
    // manual selection of trajectory configuration
    trajectory_PositionProducer = new Configuration5DOF_FromFile("cube.csv", 1000.0/20.0, true, "Trajectory", METER, TRAJECTORY_FRAME, 500);
    trajectory_PositionProducer->getNext(); // Get the first position

    Eigen::Matrix3d val;
    val << 1, 0, 0,
           0, 1, 0,
           0, 0, 1;
    //A homogeneous tranform from the Trajectory Frame to World Frame
    TrajectoryToWorld = Math::HomogeneousTransform(val, Math::Displacement(80, 160, 100, Math::WORLD_FRAME,MILLIMETER), TRAJECTORY_FRAME, WORLD_FRAME);

    goalPose = new TransformedConfiguration5DOF_producer(TrajectoryToWorld ,trajectory_PositionProducer);
    goalLocation = goalPose ->asPositionProducer();
    // ***SETUP MAGNETIC TOOL INFO
    DipoleMoment toolDipoleMoment(.126);  // Magnetic Tool info for Andrew's nice pill


    ForceVector toolWeight(0,0,-.001230, WORLD_FRAME, NEWTON); // Effective Weight given boyancy.  Can change in time from water obsorption need to modify during runs
    toolProducer = new MagneticToolProducer( toolLocation, toolDipoleMoment, goalPose, toolWeight);
    magneticTool = new SinglePerminantMagnetTool(toolProducer);

    //************************************
    // MULTIPLE OMNIMAGNET SETUP
    //************************************
    //  Requires tool setup already completed

    // Different number of Omnimagnets/Configurations have to be manually adjusted
    ///***Will want to make this freely/determined by the program or the GUI
    std::vector< Omnimagnet* > omnimagnetPointerList = {systemSetup(OmniMag0), systemSetup(OmniMag2), systemSetup(OmniMag3), systemSetup(OmniMag4), systemSetup(OmniMag5)};

    //setup the system of multiple omnimagnets: (list of pointers, magnet tool configuration, optimization settting, frame of reference)
    omnimagSystem = new MultipleOmnimagnetSystem(omnimagnetPointerList, magneticTool, MultipleOmnimagnetSystem::MINIMIZE_POWER, Math::WORLD_FRAME);

    cout << "Omnimagnet System Setup and Enabled.\n";

}

Setup::~Setup()
{
    systemSetup.inhibit();

    // Controller
    delete positionController;
    delete omnimagSystem;

}


void Setup::OFF()
{
    omnimagSystem->OFF();
}


