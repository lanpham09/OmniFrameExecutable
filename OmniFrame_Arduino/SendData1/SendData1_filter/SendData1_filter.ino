/*********************************************************

    Author: Lan Pham
    Date: May 31, 2017
    Description: Change the DEVICE_NAME variable when uploading the code to the desired Due
      //Offset to correct sensor and physically position is included in the arduino code

    
 *********************************************************/

//============================= Declare Global Variables Here =============================//
static String DEVICE_NAME("1"); //device name/id, 1 = Arm#2

static int DECIMAL_MULTIPLY = 100000;

static double SENSOR_LENGTH = 500.0;
//{Omni Rotation, Horizontal Linear, Vertical Linear, Center Rotation,TC,TC,TC}
static double SENSOR_GAIN[7] = { (double)345/40950, -(double)500 / 40950, -(double)500/ 40950, 
                                        (double)476 / 40950, 1.0, 1.0, 1.0}; //{2.094, 166.66, 166.66, 2.094, 1, 1, 1};//6.14355897
                                  //When the omnimagnet is as close to the post as possible that is
                                  //always offset but 3" from the axis of rotation, thus
                                  //77.2 mm is "zero" horizontally and 177.8 is "zero" vertically
static double SENSOR_OFFSET[7] = {90.5, -608, -520, 1.8, 0, 0, 0};
static int NUM_READINGS = 10;
static int OMNIMAGNETPin = 0;
static int HORIZONTALPin = 1;
static int VERTICALPin = 2;
static int CENTERPin = 3;

float celsius1 = 0;
float celsius2 = 0;
float celsius3 = 0;
//static int TC1 = 4;
//static int TC2 = 5;
//static int TC3 = 6;

char inByte = 0;

boolean sendData;
//================================== Begin Setup Function =================================//

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600); //Setting up baud rate (rate information is transferred)
  sendData = false;   //Initalizing sendData as not being sent yet
}

//================================== Begin Loop Function ==================================//
void loop() {
  // put your main code here, to run repeatedly:

  //UPDATE: 6/5/2017 by LP, Instead of sending and receive communication, the code currently sends its identifier number 
  //as the first number in every packet of data. Data is constantly being sent while program is running. There where 
  //issues with consistent/reliable sending and receiving of characters
  
      // Every loop, if serial is available in the due, read the character and store
      // it into "inByte". 'N' sends 'DEVICE_NAME', 'S' starts to sending data from arduino to
      // Qt, 'X' stops data being sent. This is to control when data is being sent.
      
  /*
  if ( Serial.available() > 0 )
  {
    inByte = Serial.read();

    if ( inByte == 'N')
    {
      Serial.print(DEVICE_NAME);
      //Serial.print("\n");
    }
    else if ( inByte == 'S')
    {
      sendData = true;
    }
    else if ( inByte  == 'X' )
    {
      sendData = false;
    }
    else
    {
      Serial.print("Unknown:\n" );
    }
  }//end of user input to initialize data sending



  //Data is being sent when sendData is true ('S' is received)
  if ( sendData )
*/  
  {
    //declaring variables for loop
    double voltageBits;
    double value;

    Serial.print(DEVICE_NAME); 
    Serial.print(",");
    //for loop to collect data on each sensor (A0 to A6), Currently sends data for A0 to A3 for the position sensors
    for (int i = 0 ; i < 4; i++)
    {
    analogReadResolution(12);               //adjust resolution to 12 bit for Due

    voltageBits = analogRead(i);            //read voltage from each analog pin i

    //for loop to get the average reading
    for ( int j = 1; j < NUM_READINGS; j++ )
    {
      delayMicroseconds(50);                //delay each reading by 50 microseconds
      voltageBits += analogRead(i);         // read voltage from analog pin i
    }
    //Serial.print("Voltage = "); 
    //Serial.println(voltageBits);
    value = voltageBits*SENSOR_GAIN[i] - SENSOR_OFFSET[i]; //Convert data to a physical value (degrees or mm)

    //Sending data?? Serial.print??? It works...
    Serial.print(value);
    Serial.print(",");
    Serial.flush();
    }
    //Serial.println(); 
    delay(200);                             //delay by 200 to send data evenly

  }
  //end of sending data loop


}//end of main loop
