/*********************************************************

    Author: Lan Pham
    Date: Aug 31, 2017
    Description: Change the DEVICE_NAME variable when uploading the code to the desired Due
      //Offset to correct sensor and physically position is included in the arduino code.
      Incorportates a discrete low pass filter to remove noise and considers the effect of the
      beams deflection


 *********************************************************/

//============================= Declare Global Variables Here =============================//
static String DEVICE_NAME("4"); //device name/id, indices starting at 0, "0" = Arm 1

//{Omni Rotation, Horizontal Linear, Vertical Linear, Center Rotation,TC,TC,TC}
//**********SENSOR SPECIFIC PROPERTIES*******************
static double SENSOR_GAIN[7] = { (double)345 / 4095, -(double)500 / 4095, -(double)500 / 4095,
                                 (double)474 / 4095, 1.0, 1.0, 1.0
                               };

static double SENSOR_OFFSET[7] = {28.6, -646, -520, 0, 0, 0, 0};  //[deg, mm,mm,deg]
double hor_offset = 244;                                              //[mm]
double deflection_angle = 2.18; //Beam deflection along arm can cause large deflection angles
                                //particularly at points of bending where the bending angle 
                                //can go from 1.8 to 4.5 degrees
                                
//************************************************************************

double value;
unsigned int NUM_READINGS = 20;
double hor; 

//******* LOW PASS FILTER*************

static double cut_off = 5;  //[hz] low pass cut off frequency
static double dt = 0.005;   //[s]  sample rate or rate that the system gets to the next reading

//************************************

static double pi = 3.14159;
static double tau = 1 / (cut_off * 2 * pi); //time constant [s]
static double alpha = dt / (tau + dt);      //low pass alpha calculation
static double current = 0;
static double previous[4] = {0, 0, 0, 0};
double voltageBits = 0;
unsigned int time_stamp = micros();         //unsigned int process significantly faster then doubles
unsigned int old_time = micros();           //thus if you print the time set will be lower then double

//================================== Begin Setup Function =================================//

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);     //Setting up baud rate (rate information is transferred), higher baud rate is faster sampling
                            //This is the highest setting for the Due
  analogReadResolution(12); //adjust resolution to 12 bit for Due, get analogRead from 0 to 4095 instead
                            //of 0 to 1028 for 10 bit
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

      for (int j = 0; j < NUM_READINGS; ++j) {
        current = alpha * analogRead(i) + (1 - alpha) * previous[i];  //filters input
        previous[i] = current;                                        //saves current as previous
        voltageBits += current;                                       //adds readings together to be averaged
      }

      voltageBits /= NUM_READINGS;                                    //divide by number of readings

      value = voltageBits * SENSOR_GAIN[i] - SENSOR_OFFSET[i]; //Convert data to a physical value (degrees or mm)

      if (i == 1){  //Need to save horizontal displacement to calculate deflection
        hor = value; 
      }
      //adjust for deflection in vertical direction. Depends on horizontal distance outwards and average deflection angle
      //deflection is normally greater at the beginning because the beam has been bent while holding the magnet
      if (i == 2) {
          value -= (hor - hor_offset) * tan(deflection_angle * pi / 180);
      }

      //Serial.print(voltageBits);  //prints sensor value
      Serial.print(value);        //print/send value to screen/Qt program
      Serial.print(",");          //Qt program uses "," to seperate values
      Serial.flush();             //make sure everything has been "flushed" out ie printed before moving forward
    }

    //old_time = time_stamp;
    //time_stamp = micros();
    //Serial.println("dt = ");
    //Serial.print(time_stamp - old_time);
    delay(5);                     //There needs to be a delay after printing to give the Qt program
                                  //time to receive the data in the right order

  }//end of sending data loop

}//end of main loop




