// ENTIRELY NEW SAMPLE

#include <IRremote.h>


// General
int state = 0;
unsigned long previousMillis = 0;
const long speedInterval = 50;
unsigned long currentMillis = 0;


// Motor
int motorSpeed = 0;


// // Encoder
// #define encA 0
// #define encB 0


// Motor Driver Board
/*
 - Connect the SLEEP pin on the board to 5V
 - Connect the PMODE pin on the board to GND
 - Connect the EN pin on the board to a PWM capable output
*/
#define enablePin 0 // Speed of Motor
#define phasePin 0 // Direction of Motor

// IR Sensor
#define sensorPin 5

// Laser
#define laserPin 0

void setup() {
  // General Setup
  state = 1;
  Serial.begin(9600);
  IrReceiver.begin(sensorPin, ENABLE_LED_FEEDBACK);


  // // Encoder Setup
  // pinMode(encA, INPUT);
  // pinMode(encB, INPUT);


  // Motor Driver Board Setup
  pinMode(enablePin, OUTPUT);
  analogWrite(enablePin, 0);
  pinMode(phasePin, OUTPUT);
  digitalWrite(phasePin, LOW);


  // IR Sensor Setup
  pinMode(sensorPin, INPUT_PULLUP);


  // Laser Setup
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW);
}


void runTimer() {
  currentMillis = millis();
}


void loop() {
 
  switch (state) {
    case 1: // Speed up motor
    for(;;) { // "for(;;)" runs loop forever without init/condition/increment
      Serial.print("Motor Speed = ");
      Serial.println(motorSpeed);


      runTimer();


      digitalWrite(phasePin, HIGH);
      analogWrite(enablePin, motorSpeed);

      if(currentMillis - previousMillis >= speedInterval) {
        previousMillis = currentMillis;
        motorSpeed+=1;
      }

      if(motorSpeed>=65) {
        state = 2;
        break;
      }
    }
      break;
    case 2: // Searching for IR signal
     
      for(;;) {
        digitalWrite(phasePin, HIGH);
        analogWrite(enablePin, motorSpeed);

        if (IrReceiver.decode()) {
          state = 3;
          previousMillis = currentMillis;
          IrReceiver.resume();
          break;
        }
      }


      break;
    case 3: // Slow down motor
      for(;;) {
        Serial.print("Motor Speed = ");
        Serial.println(motorSpeed);


        runTimer();


        digitalWrite(phasePin, HIGH);
        analogWrite(enablePin, motorSpeed);
        if(currentMillis - previousMillis >= speedInterval) {
          previousMillis = currentMillis;
          motorSpeed-=3;
        }
        if(motorSpeed<=0) {
          motorSpeed=0;
          state = 4;
          previousMillis = currentMillis;
          break;
        }
      }
      break;


    case 4: // Shoot laser
      for(;;) {
        runTimer();
       
        digitalWrite(laserPin, HIGH);
        if(currentMillis - previousMillis >= 2000) {
          state = 1;
          break;
        }
      }
      break;
      state = 1;
  }
}
