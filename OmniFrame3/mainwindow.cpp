#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Utilities/Units.h"
#include "Utilities/Timer.h"
#include <QDebug>
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initalization of max and min parameters for sensor error checking
    //**********************************
    for (int i = 0; i < 4; ++i){
        maxReading[i] = 0;
        minReading[i] = 4095;
    }


    /*minReading[0] = 360;
        minReading[1] = 500;
        minReading[2] = 500;
        minReading[3] = 360;
        */

    //***********************************

    //  Setups up the Omimagnets and Arduino System
    thisSetup   = new Setup();

    // Setups the demo
    thisDemo    = new Field_and_Force_Demo( thisSetup->omnimagSystem,  thisSetup->toolProducer, thisSetup->goalPose,
                                            thisSetup->positionController, MagneticField(1.5,MILLITESLA), thisSetup->desiredForce);


    //connect( &updateTimer, SIGNAL(timeout()), this, SLOT(omniSysCheck()));
    connect( ui->checkBox_omni1, SIGNAL(clicked()), this, SLOT(omniSysCheck()));
    connect( ui->checkBox_omni2, SIGNAL(clicked()), this, SLOT(omniSysCheck()));
    connect( ui->checkBox_omni3, SIGNAL(clicked()), this, SLOT(omniSysCheck()));
    connect( ui->checkBox_omni4, SIGNAL(clicked()), this, SLOT(omniSysCheck()));
    connect( ui->checkBox_omni5, SIGNAL(clicked()), this, SLOT(omniSysCheck()));
    connect( ui->checkBox_omni6, SIGNAL(clicked()), this, SLOT(omniSysCheck()));

    //Button setup(false state name, true state name, initlized state)
    //  NOTE: in order to use toggle options, GUI/InterfaceButtons has to be in .pro file
    ui->button_StartDemo ->setup("Start Demo"        , "Stop Demo"           , false);

    // Setup Timer for Trajectory Tracking Stuff for demo
    goal_Timer = new QTimer();
    mode = NOTSET;
    //connect( goal_Timer,   SIGNAL(timeout(void)), this, SLOT( updateDemo(void)) );
    //connect( ui->button_StartDemo,          SIGNAL(clicked()),  this,   SLOT( startTrajectoryMode(void) )       );
    connect(ui->button_StartDemo, SIGNAL(clicked()),this, SLOT(updateDemo(void)));


    //  TOOL LABEL AND UPDATE
    {
        ui->label_desiredPosition   ->setUnits("mm",1);
        ui->label_desiredPosition   ->value(0,0,0);
        ui->label_actualPosition    ->setUnits("mm",1);
        ui->label_actualPosition    ->value(0,0,0);
        ui->label_errorPosition     ->setUnits("mm",2);
        ui->label_errorPosition     ->value(0,0,0);
        ui->label_heading ->setUnits("",1);
        ui->label_heading ->value(0,0,0);

        connect(ui->pushButton_updateToolInfo, SIGNAL(clicked()), this, SLOT(updateToolProporties()));

        ui->label_toolWeight ->setText(QString::number(thisSetup->toolWeight.z(NEWTON)));
        cout << "Tool Weight = " << thisSetup->toolWeight.z(NEWTON) << "\n";
        ui->label_toolWeight ->setAlignment(Qt::AlignCenter);
        ui->label_toolDipole ->setNum(thisSetup->toolDipoleMoment.value(AMP_METER_SQ));
        ui->label_toolDipole ->setAlignment(Qt::AlignCenter);
        connect( &updateTimer, SIGNAL(timeout()), this, SLOT(updateToolTrajectory()) );
    }
    //  CURRENT LABEL AND UPDATE
    {
        ui->label_Current1 ->setUnits("A",2);
        ui->label_Current1 ->value(0,0,0);
        ui->label_Current2 ->setUnits("A",2);
        ui->label_Current2 ->value(0,0,0);
        ui->label_Current3 ->setUnits("A",2);
        ui->label_Current3 ->value(0,0,0);
        ui->label_Current4 ->setUnits("A",2);
        ui->label_Current4 ->value(0,0,0);
        ui->label_Current5 ->setUnits("A",2);
        ui->label_Current5 ->value(0,0,0);
        ui->label_Current6 ->setUnits("A",2);
        ui->label_Current6 ->value(0,0,0);

        connect( &updateTimer, SIGNAL(timeout()), this, SLOT(updateCurrent()) );
    }
    //  POSITION LABEL AND UPDATE
    {
        ui->label_Position1 ->setUnits("mm",2);
        ui->label_Position1 ->value(0,0,0);
        ui->label_Position2 ->setUnits("mm",2);
        ui->label_Position2 ->value(0,0,0);
        ui->label_Position3 ->setUnits("mm",2);
        ui->label_Position3 ->value(0,0,0);
        ui->label_Position4 ->setUnits("mm",2);
        ui->label_Position4 ->value(0,0,0);
        ui->label_Position5 ->setUnits("mm",2);
        ui->label_Position5 ->value(0,0,0);
        ui->label_Position6 ->setUnits("mm",2);
        ui->label_Position6 ->value(0,0,0);

        connect( &updateTimer, SIGNAL(timeout()), this, SLOT(updatePosition()) );
    }
    //  ORIENTATION LABLE AND UPDATE
    {
        ui->label_Orientation1->setNum(0.00);
        ui->label_Orientation1->setAlignment(Qt::AlignCenter);
        ui->label_Orientation2->setNum(0.00);
        ui->label_Orientation2->setAlignment(Qt::AlignCenter);
        ui->label_Orientation3->setNum(0.00);
        ui->label_Orientation3->setAlignment(Qt::AlignCenter);
        ui->label_Orientation4->setNum(0.00);
        ui->label_Orientation4->setAlignment(Qt::AlignCenter);
        ui->label_Orientation5->setNum(0.00);
        ui->label_Orientation5->setAlignment(Qt::AlignCenter);
        ui->label_Orientation6->setNum(0.00);
        ui->label_Orientation6->setAlignment(Qt::AlignCenter);

        connect( &updateTimer, SIGNAL(timeout()), this, SLOT(updateOrientation()) );
    }
    //  FIELD LABEL AND UPDATE

    //  INHIBIT BUTTON
    {
    ui->button_inhibit->setup("ENABLE AMPLIFIERS", "INHIBIT AMPLIFIERS", false);
    connect( ui->button_inhibit,    SIGNAL(clicked()),  this,   SLOT( inhibit(void) ) );}

    //  MOMENT BUTTONS
    {
        ui->button_XMoment->setup("+X Moment", "OFF", false);
        ui->button_YMoment->setup("+Y Moment", "OFF", false);
        ui->button_ZMoment->setup("+Z Moment", "OFF", false);

        connect( ui->button_XMoment,      SIGNAL(clicked()),  this,   SLOT( plusMoment(void) ) );
        connect( ui->button_YMoment,      SIGNAL(clicked()),  this,   SLOT( plusMoment(void) ) );
        connect( ui->button_ZMoment,      SIGNAL(clicked()),  this,   SLOT( plusMoment(void) ) );
    }

    //  ROTATING MAGNETIC FIELD
    {
        //Start Field Rotation Functions
        ui->button_rotatingField->setup("Start Rotating Field", "Stop Rotating Field", false);
        connect( ui->button_rotatingField,    SIGNAL(clicked()),  this,   SLOT( rotatingField() ) );

    }

    //  ***************TEMP CODE*******************

    //Trying to get the system to output the current field it is trying to induce but it is not working
    ui->label_fieldStrength->setUnits("T",3);
    ui->label_fieldStrength->value(0,0,0);

    connect( &updateTimer, SIGNAL(timeout()), this, SLOT(updateField()));   //Field does not correctly update
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateSensor()));
    updateTimer.start( 100 );

    //Gets the min, max, median and difference in error for the sensors, used to check the effect of amplifier noise
    connect(ui->button_average, SIGNAL(released()), this , SLOT(sensorError_button()));

}//end main constructor****************************************************************************************************8



//**********************************************
//  TEMPURARY/DEVELOPING
//**********************************************

void MainWindow::omniSysCheck(){
    ///Checked omnimagnets are added to the Omnimagnet System. At least one magnet has to be checked
    /// in order to use rotating magnetic field funtion or run trajectories

    if (ui->checkBox_omni1->QAbstractButton::isChecked()){
        thisSetup->omnimagSystem->addOmnimagnet(thisSetup->systemSetup(OmniMag1));}
    else{thisSetup->omnimagSystem->removeOmnimagnet(thisSetup->systemSetup(OmniMag1));}

    if (ui->checkBox_omni2->QAbstractButton::isChecked()){
        thisSetup->omnimagSystem->addOmnimagnet(thisSetup->systemSetup(OmniMag2));}
    else{thisSetup->omnimagSystem->removeOmnimagnet(thisSetup->systemSetup(OmniMag2));}

    if (ui->checkBox_omni3->QAbstractButton::isChecked()){
        thisSetup->omnimagSystem->addOmnimagnet(thisSetup->systemSetup(OmniMag3));}
    else{thisSetup->omnimagSystem->removeOmnimagnet(thisSetup->systemSetup(OmniMag3));}

    if (ui->checkBox_omni4->QAbstractButton::isChecked()){
        thisSetup->omnimagSystem->addOmnimagnet(thisSetup->systemSetup(OmniMag4));}
    else{thisSetup->omnimagSystem->removeOmnimagnet(thisSetup->systemSetup(OmniMag4));}

    if (ui->checkBox_omni5->QAbstractButton::isChecked()){
        thisSetup->omnimagSystem->addOmnimagnet(thisSetup->systemSetup(OmniMag5));}
    else{thisSetup->omnimagSystem->removeOmnimagnet(thisSetup->systemSetup(OmniMag5));}

    if (ui->checkBox_omni6->QAbstractButton::isChecked()){
        thisSetup->omnimagSystem->addOmnimagnet(thisSetup->systemSetup(OmniMag6));}
    else{thisSetup->omnimagSystem->removeOmnimagnet(thisSetup->systemSetup(OmniMag6));}
}


void MainWindow::updateToolProporties(){
//Not working
    ui->label_toolWeight ->setText(QString::number(thisSetup->toolWeight.z(NEWTON)));
    cout << "Tool Weight = " << thisSetup->toolWeight.z(NEWTON) << "\n";
    ui->label_toolDipole ->setNum(thisSetup->toolDipoleMoment.value(AMP_METER_SQ));
    cout << "Tool Dipole = " << thisSetup->toolDipoleMoment<< "\n";

}

void MainWindow::updateDemo(){
    //Just applies a vector right now

    if ( ui->button_StartDemo->getState() ){

        MagneticFieldVector desiredFieldVector(0,.0005,0,WORLD_FRAME);
        Math::Position desiredFieldPosition(0,0,0,WORLD_FRAME,MILLIMETER);
        thisSetup->omnimagSystem->applyField(desiredFieldVector,desiredFieldPosition);

    }
    else {
        MagneticFieldVector desiredFieldVector(0,0,0,WORLD_FRAME);
        Math::Position desiredFieldPosition(0,0,0,WORLD_FRAME,MILLIMETER);
        thisSetup->omnimagSystem->applyField(desiredFieldVector, desiredFieldPosition);
    }

}


//Currently can't run until trajectory files can be open correctly
/*
void MainWindow::updateDemo(){
    {

        Math::MagneticTool thisTool;
        Math::Position goalPosition;
        Math::Position toolPosition;
        Math::Displacement   errorVector;

        double Igain = .0000003;
        Distance distanceTreshold(20,MILLIMETER);        // acceptable position error

        if( thisDemo->isRunning() )
        {

            thisSetup->toolProducer->getMagneticTool( thisTool );
            thisSetup->toolLocation->getPosition( toolPosition );
            thisSetup->goalPose->getPosition( goalPosition );
            errorVector = goalPosition-toolPosition;

        }else {
            cout << "demo is not running" << endl;
            mode = NOTSET;
            goal_Timer->stop();
            //ui->button_FindWeight->setState( false );
            ui->button_StartDemo->setState( false );
            thisSetup->OFF();
        }

        switch( mode )
        {
        case NOTSET:
            break;
        case WEIGHT:
        {
            thisSetup->toolProducer->setWeight( thisTool.weight - Math::ForceVector(0,0,1,thisTool.getFrame(),NEWTON)*Math::sign(errorVector.z(MILLIMETER))*Igain );
            cout << "Tool Weight = " <<  thisTool.weight << endl;

            break;
        }

        case TRAJECTORY:

//            if( thisSetup->trajectory_PositionProducer->fileOpen() )
//            {
//                // cout << "Error Distance: " << errorVector.norm() <<endl;
//                // determine if new waypoint should be selected
//                if( errorVector.norm() < distanceTreshold )
//                {
//                    Math::Position trajectoryPos;
//                    //    thisSetup->trajectory_PositionProducer->getNext();
//                    thisSetup->trajectory_PositionProducer->getPosition(trajectoryPos);
//                }
//            }else {
                // trajectory is complete
                cout << "Trajectory is complete and trajectory file is no longer open" << endl;
                mode = NOTSET;
                goal_Timer->stop();
                startTrajectoryMode();
//            }

//            break;


        }

        return;
    }
}

void MainWindow::startTrajectoryMode()
{
    if( thisDemo->isRunning() )
    {
        //if the demo  running set everything off, reactive moment buttons and turn setup off?
        thisDemo->stop();
        //thisSetup->trajectory_PositionProducer->stop();
        goal_Timer->stop();
        mode = NOTSET;
        ui->button_XMoment->setEnabled( true );
        ui->button_YMoment->setEnabled( true );
        ui->button_ZMoment->setEnabled( true );
        thisSetup->OFF();

    } else {

        ui->button_XMoment->setEnabled( false );
        ui->button_YMoment->setEnabled( false );
        ui->button_ZMoment->setEnabled( false );


        //Math::Position omniMagPos;
        //thisSetup->systemSetup(OmniMag0)->getPosition( omniMagPos );
        Math::Position trajectoryPos;
        Math::Direction spinDirection;
        SpinVector spin = SpinVector(10);

        thisSetup->goalPose->changeProducer( thisSetup->trajectory_PositionProducer );
        //Crash due to no frame
        thisSetup->trajectory_PositionProducer->setDirection(spin.getDirection(spinDirection));
        //thisSetup->trajectory_PositionProducer->start();    //start getting trajectory inputs from file, used with Configuration5DOF_FromFile
        //thisSetup->goalPose->setPosition( thisSetup->goalPose->asPositionProducer() );
        thisSetup->goalPose->getPosition(trajectoryPos);
        cout << "Trajectory Position" << (trajectoryPos).convertToUnits(MILLIMETER) << endl;
        //cout << (TrajectoryToWorld*trajectoryPos).convertToUnits(MILLIMETER) << endl;
        //thisSetup->goalPose->setPosition( (TrajectoryToWorld*Position(0,0,2,TRAJECTORY_FRAME,CENTIMETER)).convertToUnits(MILLIMETER) );

        // Go to first position

        mode  = TRAJECTORY;
        thisDemo->start( 40 );
        //goal_timer->start( 1000.0/80.0 );

    }
    ui->button_StartDemo->setState( thisDemo->isRunning() );
    //ui->button_FindWeight->setState( false );
    return;
}

*/



void MainWindow::sensorError_button(){
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(average()));
}

void MainWindow::average(){

    int arm = 0;
    static unsigned int count = 0;
    int samples = 20;
    static double sum[4] = {0,0,0,0};
    count += 1;

    for (int i = 0; i < 4; ++i){
        if (thisSetup->Arduinos->OmniFrame[arm][i] < minReading[i]) {
            minReading[i] = thisSetup->Arduinos->OmniFrame[arm][i];
        }
        if (thisSetup->Arduinos->OmniFrame[arm][i] > maxReading[i]){
            maxReading[i] = thisSetup->Arduinos->OmniFrame[arm][i];
        }
        sum[i] += thisSetup->Arduinos->OmniFrame[arm][i];
    }
    if ( count%samples == 0 ){
        ui->Post_lcdNumber_average->display(sum[0]/samples);
        ui->Hor_lcdNumber_average->display(sum[1]/samples);
        ui->Ver_lcdNumber_average->display(sum[2]/samples);
        ui->Omni_lcdNumber_average->display(sum[3]/samples);
        sum[0] = 0;
        sum[1] = 0;
        sum[2] = 0;
        sum[3] = 0;

    }



    ui->Post_lcdNumber_min->display(minReading[0]);
    ui->Hor_lcdNumber_min->display(minReading[1]);
    ui->Ver_lcdNumber_min->display(minReading[2]);
    ui->Omni_lcdNumber_min->display(minReading[3]);

    ui->Post_lcdNumber_max->display(maxReading[0]);
    ui->Hor_lcdNumber_max->display(maxReading[1]);
    ui->Ver_lcdNumber_max->display(maxReading[2]);
    ui->Omni_lcdNumber_max->display(maxReading[3]);

    ui->Post_lcdNumber_median->display((maxReading[0]+minReading[0])/2);
    ui->Hor_lcdNumber_median->display((maxReading[1]+minReading[1])/2);
    ui->Ver_lcdNumber_median->display((maxReading[2]+minReading[2])/2);
    ui->Omni_lcdNumber_median->display((maxReading[3]+minReading[3])/2);

    ui->Post_lcdNumber_diff->display(maxReading[0]-(maxReading[0]+minReading[0])/2);
    ui->Hor_lcdNumber_diff->display(maxReading[1]-(maxReading[1]+minReading[1])/2);
    ui->Ver_lcdNumber_diff->display(maxReading[2]-(maxReading[2]+minReading[2])/2);
    ui->Omni_lcdNumber_diff->display(maxReading[3]-(maxReading[3]+minReading[3])/2);

}


void MainWindow::updateField(){
    //    //Not working as intended, the field isn't showing on the GUI
    //    //was able to amplify the value and it is not the value for the applyfield.... need to fix
    //    Math::Position test(0,0,0,WORLD_FRAME,MILLIMETER);
    //    MagneticFieldVector test1 = thisSetup->omnimagSystem->currentField(test);
    //    Math::Vector test2 = test1.asVector(MILLITESLA) * 1000;
    //    ui->label_fieldStrength->value(test2);

    }

void MainWindow::updateToolTrajectory(){
//update tool trajectory (heading, error, position) from Andrew's original multipleOmnimagnet code

    if( thisDemo->isRunning() && thisSetup->desiredForce->historyLength() > 0 )
    {
        ui->label_controlForce ->value(thisSetup->desiredForce->value().value*1000);
    } else {
        ui->label_controlForce ->value(0,0,0);
    }

    //**Tool Position Update
    Math::Position posDes;
    Math::Position posTool;

    bool pos1good, pos2good;
    //Trajectory file is not opening correctly resulting in tool information not being updated correctly
    if( ( pos1good = thisSetup->goalPose->getPosition( posDes )) )
    {
        ui->label_desiredPosition->value( posDes.convertToUnits(MILLIMETER), MILLIMETER );
        ui->label_heading->value(thisSetup->goalPose->returnDirection().asVector());
    } else
        ui->label_actualPosition->value( Math::Zero() );

    if ( (pos2good = thisSetup->toolLocation->getPosition( posTool ) ))
        ui->label_actualPosition->value( posTool.convertToUnits(MILLIMETER), MILLIMETER );
    else
        ui->label_actualPosition->value( Math::Zero() );

    if ( thisDemo->isRunning() )
        ui->label_errorPosition->value( (posDes - posTool).convertToUnits(MILLIMETER), MILLIMETER );
    else
        ui->label_errorPosition->value( Math::Zero() );

}

//*************************
//  FUNCTIONS
//*************************

//induces a moment at the specified Omnimagnet
void MainWindow::plusMoment()
{
    ///Slot to run Moment Buttons
    ///Descprition: Determining which button was pressed by comparing who the sender of the sigal and applys a moment vector of 50 for x,y or z
    ///     flag keeps track of which button was pushed. Direction of moment can be adjust by change value while direction can be
    ///     changed by distributing across i,j,k accordingly.

    int i = 0; int j = 0; int k = 0; bool pressed = false; int flag = 0;

    if (sender() == ui->button_XMoment){
        pressed =  ui->button_XMoment->getState();
        flag = 1;
        i = 50;
    }
    else if (sender() == ui->button_YMoment){
        pressed =  ui->button_YMoment->getState();
        flag = 2;
        j = 50;
    }
    else if (sender() == ui->button_ZMoment){
        pressed =  ui->button_ZMoment->getState();
        flag = 3;
        k = 50;
    }

    if( pressed )
    {
        thisSetup->systemSetup(OmniMag0)->dipoleMoment(Math::DipoleMomentVector(0,0,0, Math::WORLD_FRAME));

        if (ui->spin_whichOmni->value() == 1)
            thisSetup->systemSetup(OmniMag1)->dipoleMoment(Math::DipoleMomentVector(i,j,k, Math::WORLD_FRAME));
        else if (ui->spin_whichOmni->value() == 2)
            thisSetup->systemSetup(OmniMag2)->dipoleMoment(Math::DipoleMomentVector(i,j,k, Math::WORLD_FRAME));
        else if (ui->spin_whichOmni->value() == 3)
            thisSetup->systemSetup(OmniMag3)->dipoleMoment(Math::DipoleMomentVector(i,j,k, Math::WORLD_FRAME));
        else if (ui->spin_whichOmni->value() == 4)
            thisSetup->systemSetup(OmniMag4)->dipoleMoment(Math::DipoleMomentVector(i,j,k, Math::WORLD_FRAME));
        else if (ui->spin_whichOmni->value() == 5)
            thisSetup->systemSetup(OmniMag5)->dipoleMoment(Math::DipoleMomentVector(i,j,k, Math::WORLD_FRAME));
        else if (ui->spin_whichOmni->value() == 6)
            thisSetup->systemSetup(OmniMag6)->dipoleMoment(Math::DipoleMomentVector(i,j,k, Math::WORLD_FRAME));

        ui->button_XMoment->setState( false );
        ui->button_YMoment->setState( false );
        ui->button_ZMoment->setState( false );
        if (flag == 1){
            ui->button_XMoment->setState( true );}
        else if (flag == 2){
            ui->button_YMoment->setState( true );}
        else if (flag == 3){
            ui->button_ZMoment->setState( true );}

        //ui->button_FindWeight->setEnabled( false );
        //ui->button_StartDemo->setEnabled( false );

    } else {
        thisSetup->systemSetup(OmniMag0)->dipoleMoment(Math::DipoleMomentVector(0,0,0, Math::WORLD_FRAME));
        thisSetup->systemSetup(OmniMag1)->dipoleMoment(Math::DipoleMomentVector(0,0,0, Math::WORLD_FRAME));
        thisSetup->systemSetup(OmniMag2)->dipoleMoment(Math::DipoleMomentVector(0,0,0, Math::WORLD_FRAME));
        thisSetup->systemSetup(OmniMag3)->dipoleMoment(Math::DipoleMomentVector(0,0,0, Math::WORLD_FRAME));
        thisSetup->systemSetup(OmniMag4)->dipoleMoment(Math::DipoleMomentVector(0,0,0, Math::WORLD_FRAME));
        thisSetup->systemSetup(OmniMag5)->dipoleMoment(Math::DipoleMomentVector(0,0,0, Math::WORLD_FRAME));
        thisSetup->systemSetup(OmniMag6)->dipoleMoment(Math::DipoleMomentVector(0,0,0, Math::WORLD_FRAME));

        if (flag == 1){
            ui->button_XMoment->setState( false );}
        else if (flag == 2){
            ui->button_YMoment->setState( false );}
        else if (flag == 3){
            ui->button_ZMoment->setState( false );}

        //ui->button_FindWeight->setEnabled( true );
        //ui->button_StartDemo->setEnabled( true );
    }
}

//support function to get field location for other functions
double* MainWindow::getFieldLocation(){
    //make fieldLocatino static since C++ can only return the pointer to the location
    //by making the variable static it prevents the values from being lost when you exit the function

    static double fieldLocation[3];
    fieldLocation[0] = ui->doubleSpinBox_xFieldLocation->value();
    fieldLocation[1] = ui->doubleSpinBox_yFieldLocation->value();
    fieldLocation[2] = ui->doubleSpinBox_zFieldLocation->value();

    return fieldLocation;
}

//induce a rotating magnetic field with specified strength, axis of rotation and rotation speed
void MainWindow::rotatingField(){
///Connects with button_rotatingField to induce a rotating field with field stength, field location, and speed of rotation set from the GUI.

    double x = ui->doubleSpinBox_xRotatingField->value()/1000.0;
    double y = ui->doubleSpinBox_yRotatingField->value()/1000.0;
    double z = ui->doubleSpinBox_zRotatingField->value()/1000.0;

    int a; int b; int c;

    if (ui->checkBox_xAxis->QAbstractButton::isChecked()){ a = 1; }
    else {a = 0;}
    if (ui->checkBox_yAxis->QAbstractButton::isChecked()){ b = 1; }
    else {b = 0;}
    if (ui->checkBox_zAxis->QAbstractButton::isChecked()){ c = 1; }
    else {c = 0;}

    double rotationRate = ui->doubleSpinBox_rotationRate->value();

    //spin = SpinningVector( Vector(x,y,z), Vector(a,b,c), rotationRate );
    //This line works perfectly: spin = SpinningVector(Vector(0,0.0001, 0), Vector(0,0,1),1);
    //This still works: spin = SpinningVector(Vector(x,y,z), Vector(0,0,1),1);
    spin = SpinningVector( Vector(x,y,z), Vector(a,b,c), rotationRate);

    if (ui->button_rotatingField->getState()){
        spin.start();
        //have to update the spin vector using connect, it doesn't update on its own
        connect( &updateRotation, SIGNAL(timeout()), this, SLOT(rotate()));

        updateRotation.start(100);

    }
    else {
        spin.stop();
        double *xyz = getFieldLocation();
        MagneticFieldVector desiredFieldVector(0,0,0,WORLD_FRAME);
        Math::Position desiredFieldPosition(xyz[0],xyz[1],xyz[3],WORLD_FRAME,MILLIMETER);
        thisSetup->omnimagSystem->applyField(desiredFieldVector, desiredFieldPosition);
        updateRotation.stop();
    }
}

//support function for rotatingField which updates the SpinVector
void MainWindow::rotate(){
///Function paired with the rotatingField() button which updates the SpinVector to have the field rotate

    Math::Vector V = spin;  //casting the SpinningVector to a Vector will automatically update the SpinningVector
    //cout << "SpinningVector = " << V;
    MagneticFieldVector desiredFieldVector(V, WORLD_FRAME);

    double *xyz = getFieldLocation();

    thisSetup->omnimagSystem->applyField(desiredFieldVector, Math::Position(xyz[0],xyz[1],xyz[2],WORLD_FRAME,MILLIMETER));
}

//*****************
//  LABEL UPDATE
//*****************

void MainWindow::updateSensor(){
    ///Function to updated LCD screen. Because each LCD screen is it's own variable there isn't a
    ///to loop this process

    int arm = ui->spin_whichArm->value() -1 ;
    ui->Post_lcdNumber->display(thisSetup->Arduinos->OmniFrame[arm][0]);
    ui->Hor_lcdNumber->display(thisSetup->Arduinos->OmniFrame[arm][1]);
    ui->Ver_lcdNumber->display(thisSetup->Arduinos->OmniFrame[arm][2]);
    ui->Omni_lcdNumber->display(thisSetup->Arduinos->OmniFrame[arm][3]);

}

void MainWindow::updateOrientation(){

    double pi = 3.14159;
    QString units = "  deg";
    ui->label_Orientation1->setText(QString::number(thisSetup->omni_orient[0]*180/pi) + units);
    ui->label_Orientation2->setText(QString::number(thisSetup->omni_orient[1]*180/pi) + units);
    ui->label_Orientation3->setText(QString::number(thisSetup->omni_orient[2]*180/pi) + units);
    ui->label_Orientation4->setText(QString::number(thisSetup->omni_orient[3]*180/pi) + units);
    ui->label_Orientation5->setText(QString::number(thisSetup->omni_orient[4]*180/pi) + units);
    ui->label_Orientation6->setText(QString::number(thisSetup->omni_orient[5]*180/pi) + units);
}

void MainWindow::updatePosition(){
    ui->label_Position1->value( thisSetup->systemSetup(OmniMag1)->returnPosition() );
    ui->label_Position2->value( thisSetup->systemSetup(OmniMag2)->returnPosition() );
    ui->label_Position3->value( thisSetup->systemSetup(OmniMag3)->returnPosition() );
    ui->label_Position4->value( thisSetup->systemSetup(OmniMag4)->returnPosition() );
    ui->label_Position5->value( thisSetup->systemSetup(OmniMag5)->returnPosition() );
    ui->label_Position6->value( thisSetup->systemSetup(OmniMag6)->returnPosition() );
}

//updates the display for current to the omnimagnets and updates the max current bar
void MainWindow::updateCurrent(){
    static double maxCurrent = 0;
    static double omni_MaxCurrent[6] = {0,0,0,0,0,0};

    ui->label_Current1 ->value(thisSetup->systemSetup(OmniMag1)->commandedCurrent());
    Vector A = thisSetup->systemSetup(OmniMag1)->commandedCurrent();
    omni_MaxCurrent[0] = std::max(std::abs(A.x_c()), std::max(std::abs(A.y_c()), std::abs(A.z_c())));

    ui->label_Current2 ->value(thisSetup->systemSetup(OmniMag2)->commandedCurrent());
    A = thisSetup->systemSetup(OmniMag2)->commandedCurrent();
    omni_MaxCurrent[1] = std::max(std::abs(A.x_c()), std::max(std::abs(A.y_c()), std::abs(A.z_c())));

    ui->label_Current3 ->value(thisSetup->systemSetup(OmniMag3)->commandedCurrent());
    A = thisSetup->systemSetup(OmniMag3)->commandedCurrent();
    omni_MaxCurrent[2] = std::max(std::abs(A.x_c()), std::max(std::abs(A.y_c()), std::abs(A.z_c())));

    ui->label_Current4 ->value(thisSetup->systemSetup(OmniMag4)->commandedCurrent());
    A = thisSetup->systemSetup(OmniMag4)->commandedCurrent();
    omni_MaxCurrent[3] = std::max(std::abs(A.x_c()), std::max(std::abs(A.y_c()), std::abs(A.z_c())));

    ui->label_Current5 ->value(thisSetup->systemSetup(OmniMag5)->commandedCurrent());
    A = thisSetup->systemSetup(OmniMag5)->commandedCurrent();
    omni_MaxCurrent[4] = std::max(std::abs(A.x_c()), std::max(std::abs(A.y_c()), std::abs(A.z_c())));

    ui->label_Current6 ->value(thisSetup->systemSetup(OmniMag6)->commandedCurrent());
    A = thisSetup->systemSetup(OmniMag6)->commandedCurrent();
    omni_MaxCurrent[5] = std::max(std::abs(A.x_c()), std::max(std::abs(A.y_c()), std::abs(A.z_c())));

    maxCurrent = *std::max_element(omni_MaxCurrent, omni_MaxCurrent+6);
    ui->progressBar_current->setValue(maxCurrent/15.0*100.0);



}


//****************
//  INHIBIT
//****************

void MainWindow::inhibit(){
    ///Inhibits the system (turns if off) if any of the stricts occur. Look inhibit class (it's somewhere I found it before ...)
    ///Current might be commanded but the amplifiers won't apply the current if inhibited
    cout << "Enable System " << ui->button_inhibit->getState() << endl;
    if( !ui->button_inhibit->getState() )
    {
        cout << "INHIBIT SYSTEM" << endl;
        thisSetup->systemSetup.inhibit();
    }else
    {
        cout << "ENABLE SYSTEM" << endl;
        thisSetup->systemSetup.enable();
        cout << "Sucess" << thisSetup->systemSetup.isEnabled() << endl;
    }

    ui->button_inhibit->setState( thisSetup->systemSetup.isEnabled() );
}

//*******************
//  DECONSTRUCTOR
//*******************

MainWindow::~MainWindow()
{
    thisSetup->OFF();
    delete thisSetup;
    delete ui;
    delete goal_Timer;
}

