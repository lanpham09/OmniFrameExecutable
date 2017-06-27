#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Utilities/Units.h"
#include "Utilities/Timer.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    //  Setups up the Omimagnets
    //

    thisSetup   = new Setup();
    //
    //  Inhibit Button Setup
    //

    //NOTE: in order to use toggle options, GUI/InterfaceButtons has to be in .pro file
    ui->button_inhibit->setup("ENABLE", "INHIBIT", false);
    //Connects to the inhibit button
    connect( ui->button_inhibit,    SIGNAL(clicked()),  this,   SLOT( inhibit(void) ) );

    //
    //  Current Label Setup
    //

    ui->label_Current ->setUnits("A",2);
    ui->label_Current ->value(0,0,0);
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
    updateTimer.start( 100 );

    //
    //  Moment Buttons Setup
    //

    ui->button_XMoment->setup("+X Moment", "OFF", false);
    ui->button_YMoment->setup("+Y Moment", "OFF", false);
    ui->button_ZMoment->setup("+Z Moment", "OFF", false);

    connect( ui->button_XMoment,      SIGNAL(clicked()),  this,   SLOT( plusMoment(void) ) );
    connect( ui->button_YMoment,      SIGNAL(clicked()),  this,   SLOT( plusMoment(void) ) );
    connect( ui->button_ZMoment,      SIGNAL(clicked()),  this,   SLOT( plusMoment(void) ) );

    //
    //  Position Label Setup
    //

    ui->label_Position ->setUnits("mm",2);
    ui->label_Position ->value(0,0,0);
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


    //
    //  Orientation Update
    //

    ui->label_Orientation->setNum(0.00);
    ui->label_Orientation->setAlignment(Qt::AlignCenter);
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

}

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

void MainWindow::updateOrientation(){

}

void MainWindow::updateCurrent(){
    ///Updates what the commanded current to the Omnimagnets are

    ui->label_Current ->value(thisSetup->systemSetup(OmniMag0)->commandedCurrent());
    ui->label_Current1 ->value(thisSetup->systemSetup(OmniMag1)->commandedCurrent());
    ui->label_Current2 ->value(thisSetup->systemSetup(OmniMag2)->commandedCurrent());
    ui->label_Current3 ->value(thisSetup->systemSetup(OmniMag3)->commandedCurrent());
    ui->label_Current4 ->value(thisSetup->systemSetup(OmniMag4)->commandedCurrent());
    ui->label_Current5 ->value(thisSetup->systemSetup(OmniMag5)->commandedCurrent());
    ui->label_Current6 ->value(thisSetup->systemSetup(OmniMag6)->commandedCurrent());

}


//****************
//  INHIBIT
//***************

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


MainWindow::~MainWindow()
{
    thisSetup->OFF();
    delete thisSetup;
    delete ui;
}

