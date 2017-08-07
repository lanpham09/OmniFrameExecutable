/*********************************************************

    Author: Lan Pham
    Date: July 1, 2017
    Description: Change the DEVICE_NAME variable when uploading the code to the desired Due
      //Offset to correct sensor and physically position is included in the arduino code. 
      Incorportates a discrete low pass filter to remove noise


 *********************************************************/

//============================= Declare Global Variables Here =============================//
static String DEVICE_NAME("1"); //device name/id, 1 = Arm#2

//{Omni Rotation, Horizontal Linear, Vertical Linear, Center Rotation,TC,TC,TC}
static double SENSOR_GAIN[7] = { (double)345 / 4095, -(double)500 / 4095, -(double)500 / 4095,
                                 (double)476 / 4095, 1.0, 1.0, 1.0
                               };

static int SENSOR_OFFSET[7] = {90.5, -608, -520, 1.8, 0, 0, 0};
static int NUM_READINGS = 75;
int value;

//******* CUT OFF FREQ AND DT*************
int cut_off = .5;  //[hz]
double dt = 0.00007;  //obtained from serial print test

//**************************

static double pi = 3.14159;
double tau = 1 / (cut_off * 2 * pi); //time constant [s]
double alpha = dt / (tau + dt);
double current = 0;
double previous[4] = {0, 0, 0, 0};
double voltageBits = 0;
int time_stamp = micros();
int old_time = micros();

//================================== Begin Setup Function =================================//

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); //Setting up baud rate (rate information is transferred)
  analogReadResolution(12);               //adjust resolution to 12 bit for Due, get analogRead from 0 to 4095

}

//================================== Begin Loop Function ==================================//
void loop() {
  // put your main code here, to run repeatedly:

  {
    Serial.print(DEVICE_NAME);
    Serial.print(",");
    for (int i = 0 ; i < 4; i++)
    {
      voltageBits = 0;

      for ( int j = 1; j <= NUM_READINGS; j++ )
      {
        //time between each sample is ~70 microsec = 0.00007 sec
        current = alpha * analogRead(i) + (1 - alpha) * previous[i];
        previous[i] = current;
        voltageBits += current;
//        old_time = time_stamp;
//        time_stamp = micros();
//        Serial.println(time_stamp-old_time);

      }
      
      voltageBits /= NUM_READINGS;
      value = voltageBits * SENSOR_GAIN[i] - SENSOR_OFFSET[i]; //Convert data to a physical value (degrees or mm)

      //Serial.println(analogRead(i));
      Serial.print(voltageBits);
      Serial.print(",");
      //old_time = time_stamp;
      //time_stamp = micros();
      //Serial.print(" dt = ");
      //Serial.print(time_stamp-old_time);

      Serial.flush();
    }

//    old_time = time_stamp;
//    time_stamp = micros();
//    Serial.println(time_stamp - old_time);
    delay(150);                             //delay by 200 milliseconds [.2 s] to send data evenly

  }
  //end of sending data loop


}//end of main loop
