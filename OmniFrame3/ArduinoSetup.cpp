#include "ArduinoSetup.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>
#include <vector>

//General class info can be found on .h file.

ArduinoSetup::ArduinoSetup(){
    //Constructor that will run all arduino Dues connect to computer

    for (int i = 0; i < 6; ++i){
        arduino_is_available[0] = false;   //initialize port availablity to false
        arduino_port_name[0] == "";        //initalize port name to nothing
        serialBuffer[i] = "";              //initalize serialBuffer to nothing
        for (int j = 0; j < 4 ; ++j){
            OmniFrame[i][j] = 0;           //initalize all OmniFrame values to 0
        }
    }

    //Declaring an array of Arduinos
    arduino[6] = new QSerialPort;

    port_assignment();
    configure_serialport();
    connect_serialport();
}

void ArduinoSetup::port_assignment(){

    ///*Runs though each available port on the computer (include ones not connected to Dues)
    ///*and what the vendor and product ID of that connection is. Using the information from
    ///* find_arduino_IDinfo() function the specific product ID and vendor IDs (not more than one)
    ///* a Due is idenfied. The specific port of that Due is stored into an array and the availablity
    ///* of that slot number is made true.
    ///*///
    for (int i = 0; i <= number_of_arduinos; ++i) {
        number_of_arduinos = -1;
        foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
            if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()) {

                number_of_arduinos = number_of_arduinos + 1;

                if ( (serialPortInfo.vendorIdentifier() == arduino_due_vendor_id) ||
                     (serialPortInfo.vendorIdentifier() == arduino_due_vendor_id_2)) {

                    if (serialPortInfo.productIdentifier() == arduino_due_product_id ) {
                        arduino_port_name[number_of_arduinos] = serialPortInfo.portName();
                        arduino_is_available[number_of_arduinos] = true;
                    }
                }
            }
        }
    }

}//*** End of Port Assignment*********

void ArduinoSetup::configure_serialport(){
    ///*INFO: Connects each avaiable port to a Due*/
    for (int i = 0; i <= number_of_arduinos; ++i){
        if(arduino_is_available[i]) {
            //if due is available from the port_assignment function than make QSerialPort to connect to
            arduino[i] = new QSerialPort;

            //recommended initalization sequence for arduino configuration
            arduino[i]->setPortName(arduino_port_name[i]);
            arduino[i]->open(QSerialPort::ReadWrite);         //Open the port to send and read data
            arduino[i]->setBaudRate(QSerialPort::Baud115200);   //increase baud rate for Due to increase speed
            arduino[i]->setDataBits(QSerialPort::Data8);     //Size of a bite. 8 is the highest option even though Due operates at 12  bits
            arduino[i]->setParity(QSerialPort::NoParity);
            arduino[i]->setStopBits(QSerialPort::OneStop);
            arduino[i]->setFlowControl(QSerialPort::NoFlowControl);

        }
        else {
            //if no port is available then give warning
            QMessageBox::warning(NULL, "Port Error", "Couldn't find the Arduino for Port!");
        }
    }
}//**End of connect to arduino

void ArduinoSetup::connect_serialport(){
    for (int i = 0; i <= number_of_arduinos; ++i){
        if(arduino_is_available[i]) {
            ///connect to arduino and start reading data. From that data red run it through the readSerial
            ///slot function. Assigning it to a bool allows for verification of connection.
            bool check = QObject::connect(arduino[i], SIGNAL(readyRead()), this, SLOT(readSerial()));   //function to read data
            if (check)
                qDebug() << i+1 << " arduino(s) are connected and sending data";
        }
    }
}

void ArduinoSetup::readSerial()
///*Reads data from Due*/
{
    for (int i = 0; i <= number_of_arduinos ; ++i){

        if (sender() == arduino[i] ){
            //compares first input to connect and reads data from that specific arduino

            //Reads all data coming from arduino
            serialData[i] = arduino[i]->readAll();
            //takes the data and puts it into a long string one after the other
            serialBuffer[i] += QString::fromStdString(serialData[i].toStdString());

            //splits the serial buffer by the character "," to get individual values
            bufferSplit[i] = serialBuffer[i].split(",");

            if (bufferSplit[i].length() < 41 ){
                //bufferSplit length is very important! if the length is wrong than the data will
                //not print out consistantly. Must be adjust for number of receiving inputs
                //(# of values + 1 for the "" at the end)

                serialData[i] = arduino[i]->readAll();
                serialBuffer[i] += QString::fromStdString(serialData[i].toStdString());

            }
            else if (bufferSplit[i].length() > 41 ){

                serialBuffer[i] = "" ;    //resets serial buffer if the length becomes too long (shifted/loss data)
            }
            else {
                //When the bufferSplit has all of the desired values it can be send to updateLCD

                arduino_id[i] = bufferSplit[i][0].toInt();
                //qDebug() <<"Arduino " << arduino_id[i] ;
                //qDebug() << bufferSplit[i] ;
                //Organized the position data for each arm in order from arm #1-6 (with index starting at 0)
                //[post angel, horizontal distance mm, vertical distance mm, Omni angle]


                OmniFrame[arduino_id[i]][0] = bufferSplit[i][1].toDouble();
                OmniFrame[arduino_id[i]][1] = bufferSplit[i][2].toDouble();
                OmniFrame[arduino_id[i]][2] = bufferSplit[i][3].toDouble();
                OmniFrame[arduino_id[i]][3] = bufferSplit[i][4].toDouble();

                //MainWindow::updateLCD(Due->arduino_id[i], Due->bufferSplit[i]);
                serialBuffer[i] = "" ;         //resets serial buffer so its not infinitely long

            }
        }
    }

}//*** End of Read and Send to LCD Screen

void ArduinoSetup::find_arduino_IDinfo(){

    ///Function to finds the vendor and product ID for the arduino. This is used to
    ///insure other device connected to the computer doesn't get incorrected declared
    ///as an arduion.

    ///Vendor ID varies by manufactor of Arduino and Product ID will change with product type (Due or Uno)
    ///Single usage once info is found

    qDebug() << "Number of available ports: "  << QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
        if(serialPortInfo.hasVendorIdentifier()){
            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        }
        qDebug() << "Has product ID: " << serialPortInfo.hasProductIdentifier();
        if(serialPortInfo.hasProductIdentifier()){
            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        }
    }

}

ArduinoSetup::~ArduinoSetup(){
    ///Destructor
    ///if the arduino port is open we also have to close it
    for (int i = 0; i < number_of_arduinos; ++i ){
        if (arduino[i]->isOpen()) {
            arduino[i]->close();
        }
    }
}
