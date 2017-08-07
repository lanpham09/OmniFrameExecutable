#ifndef ARDUINO_H
#define ARDUINO_H

#include <QDialog>
#include <QSerialPort>
#include <Producers/Configuration6DOF_Producer.h>

/*ArduinoSetup
 *
 * Description: Setups up arduino serial communication and configurates that information into Andrew's Configuration6DOFProducer
 *              format
*/

class ArduinoSetup : public QObject
{
    Q_OBJECT        //marco necessary to use QObect functions
    //run qmake to avoid 'vtable for ...' error
public:

    ArduinoSetup();
    ~ArduinoSetup();
    //These two must be public
    int number_of_arduinos;
    bool arduino_is_available[6];

    //array of serialports for arduino
    QSerialPort *arduino[6];

    //Arduino Data Variables
    QString arduino_port_name[6];
    int arduino_id[6];
    QByteArray serialData[6];
    QString serialBuffer[6];
    QStringList bufferSplit[6];


    //arduino functions
    void port_assignment();
    void configure_serialport();
    void connect_serialport();
    void convert_to_confProducer();
    double* transform( double[4] );

    //Sorted OmniFrame data into [ post, hor, ver, omni]

    double OmniFrame[6][4];
    Configuration6DOF_Producer OmniFrameConfig[6];

//    //Low Pass Filter Setup
//    double previous[6][4];
//    double dt;
//    double cut_off;
//    double tau;
//    double alpha;


private:
    //defining the arduino to connect to as a Due with two possible vender IDs
    //WARNING: DIFFERENT ARDUINO WILL NEED UPDATED VENDOR ID AND PRODUCT ID #S
    void find_arduino_IDinfo();
    static const quint16 arduino_due_vendor_id = 10755;  //different arduino vendors thus two different IDs
    static const quint16 arduino_due_vendor_id_2 = 9025;
    static const quint16 arduino_due_product_id = 61;    //all Due have same product ID


private slots:

    void readSerial();

};

#endif // ARDUINO_H
