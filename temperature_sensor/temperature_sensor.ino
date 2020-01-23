#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <avr/pgmspace.h>
Adafruit_ADS1115 ads1115;



//CHANGE THESE TEMPERATURES BASED ON THE RANGES GIVEN AT THE COMPETITION
double lowTemp = 31.0;
double lowMidTemp = 40.0;
double midHighTemp = 40.0;
double highTemp = 57.0;
bool nationals = true; //Set true if at nationals. Disables/Enables the double led light up thing
//ENDS HERE ------------------------------------------------------------------------------

//Constants for voltage to temperature equation:
const double A = 340.279;
const double B = -39.4879;
const double C = 0.0635671;
//ENDS HERE-------------------------------------

int r = 11;
int g = 12;
int b = 13;
int x = 9; // This pin is not connected to anything intentionally

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HERE IS HOW YOU USE THIS:                                                                                      //
//r = red, g = green, b = blue, x = no LED                                                                       //
//will light up all LEDs in the column at the same index as the nearest range that the temperature is LESS THAN. //
//                                                                                                               //
//For example if the ranges were:                                                                                //
//                                                                                                               //
//{10.0, 20.0, 30.0, 40.0}                                                                                       //
//                                                                                                               //
//and the LEDs were:                                                                                             //
//                                                                                                               //
//{b, b, g, r, r},                                                                                               //
//{x, g, x, g, x},                                                                                               //
//{x, x, x, x, x}                                                                                                //
//                                                                                                               //
//and the temperature was 15.0                                                                                   //
//                                                                                                               //
//since 15.0 is greater than 10.0 and less than 20.0, and 20.0 is the SECOND element in ranges[]                 //
//the BLUE and GREEN LEDs would light up because they are in the SECOND column in LEDs[][]                       //
//                                                                                                               //
//The amount of columns in LEDs should always equal the number of elements in ranges + 1                         //
//As long as the above is true you can add as many or as few ranges as you like                                  //
//                                                                                                               //
//If you have any questions or this doesn't work text me @ 610-662-3339                                          //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double ranges[] = {10.0, 20.0, 30.0, 40.0};
int LEDs[3][sizeof(ranges) / sizeof(ranges[0]) + 1] = {
                  {b, b, g, r, r},
                  {x, g, x, g, x},
                  {x, x, x, x, x}
    };
//END--------------------------------------------------------------------------------------------------------------
                
const double tuningConstant = 1.0;//change this to add an offset to temperature reading

double temperature = 0.0;

double getTemp(double voltage) {
  return (A + B * log(((voltage / 5) * 8850) / (1 - (voltage / 5))) + C * pow(log(((voltage / 5) * 8850) / (1 - (voltage / 5))), 3)) * tuningConstant;
}

void setup() {

  Serial.begin(115200);
  ads1115.begin(); // Initialize ads1115
  ads1115.setGain(GAIN_ONE);
  Serial.println("Initalized");

  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(x, OUTPUT);
}

void loop() {

 int16_t adc0, adc1, adc2, adc3;
 adc0 = ads1115.readADC_SingleEnded(0); 
 
 
  Serial.print("Voltage: ");
  float val = adc0 * .000125; 
  Serial.println(val);

  temperature = getTemp(val);

  //Calls method
  lightLED();

  
  Serial.print("Temperature(C): ");
  Serial.println(temperature);

  int sensorValue = analogRead(A0);
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  Serial.println("----------------------"); //This is just a divider to organize the serial monitor.

  delay(500);
}

void lightLED(){
  //Resets pins and variables
  bool breakLoop = false;
  digitalWrite(r, LOW);
  digitalWrite(g, LOW);
  digitalWrite(b, LOW);
  digitalWrite(x, LOW);
  
  Serial.println(ranges[sizeof(ranges) / sizeof(ranges[0])]);
  
  //If the temperature is greater than the last value in the ranges array
  if(temperature > ranges[sizeof(ranges) / sizeof(ranges[0])]){
    //lights up the LEDs in the last column of the LEDs array
    for (int y; y <= 3; y++){
        digitalWrite(LEDs[y][sizeof(ranges) / sizeof(ranges[0]) + 1], HIGH);
      }
  } else {
  //goes through the ranges array and accesses its values
    for(int x = 0; x < sizeof(ranges) / sizeof(ranges[0]) + 1; x++){
      //If the temperature is less than the range in ranges at index x
       if(temperature < ranges[x]){
         //light up corresponding LEDs
         for (int y; y <= 3; y++){
            digitalWrite(LEDs[y][x], HIGH);
            Serial.print(LEDs[y][x]);
            breakLoop = true;
          }
        }
        if(breakLoop){
          break;
        }
      }
    }
  }

/*void lightLED(){
  Serial.print("Light: ");
  if(temperature < lowTemp){
    digitalWrite(b, HIGH);
    digitalWrite(g, LOW);
    digitalWrite(r, LOW);

    Serial.println("Blue");
  }
  else if(temperature < lowMidTemp && nationals){
    digitalWrite(b, HIGH);
    digitalWrite(g, HIGH);
    digitalWrite(r, LOW);

    Serial.println("Blue Green");
  }
  else if(temperature < midHighTemp && nationals){
    digitalWrite(b, LOW);
    digitalWrite(g, HIGH);
    digitalWrite(r, LOW);

    Serial.println("Green");
  }
  else if(temperature < highTemp){
    if(nationals){
      digitalWrite(b, LOW);
      digitalWrite(g, HIGH);
      digitalWrite(r, HIGH);
      
      Serial.println("Green Red");
    } else {
      digitalWrite(b, LOW);
      digitalWrite(g, HIGH);
      digitalWrite(r, LOW);

      Serial.println("Green");
    }
  }
  else{ //This means that none of the previous conditions are met
    digitalWrite(b, LOW);
    digitalWrite(g, LOW);
    digitalWrite(r, HIGH);

    Serial.println("Red");
  }
}*/
