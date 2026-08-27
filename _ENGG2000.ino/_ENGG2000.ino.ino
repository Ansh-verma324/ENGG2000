//Trial 2 


/*
 * Arduino Uno + DRV8874 + GB37Y3530 DC Motor with Encoder
 */

// ============================================
// PIN DEFINITIONS
// ============================================

const int enPin = 3;    // Speed control (PWM) → DRV8874 EN/IN1
const int phPin = 8;    // Direction control → DRV8874 PH/IN2
const int sleepPin = 9; // Wake up driver

const int encA = 2;     // Encoder Channel A (Interrupt pin)
const int encB = 4;     // Encoder Channel B

const int receiverPin = 10; // IR data pin (not used yet)

const int laserPin = 7; //Pin for the laser

// ============================================
// VARIABLES
// ============================================

volatile long encoderCount = 0; 
int motorSpeed = 255;   // Full speed

// ============================================
// ENCODER INTERRUPT SERVICE ROUTINE
// ============================================
void encoderISR() {
  if (digitalRead(encB) == HIGH) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}

// ============================================
// SETUP
// ============================================
void setup() {

  Serial.begin(9600);
 
  // Motor control pins
  pinMode(enPin, OUTPUT);
  pinMode(phPin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
 
  // Wake up the driver
  digitalWrite(sleepPin, HIGH);
  delay(10);
 
  // Encoder pins
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);

  //Laser pin
  pinMode(laserPin, OUTPUT);
 
  //IR pin
  pinMode(receiverPin, INPUT);

  // Attach interrupt - encoderISR is now declared
  attachInterrupt(digitalPinToInterrupt(encA), encoderISR, CHANGE);
 
  Serial.println("Motor Control Ready!");
  Serial.println("------------------------------------------");
  delay(1000);

}

// ============================================
// MAIN LOOP
// ============================================

void loop() {

  Serial.println("FORWARD");
  digitalWrite(laserPin, LOW); // Off
  digitalWrite(phPin, HIGH);   // Forward direction
  analogWrite(enPin, motorSpeed);
  encoderCount = 0;             // Reset encoder count
  
  int state = digitalRead(receiverPin); 
  if (state == LOW) {
    Serial.println("STOP");
    analogWrite(enPin, 0);        // Brake
    digitalWrite(laserPin, HIGH); // On
    Serial.println("IR DETECTED");
    delay(5000);                  // Stop for 5 seconds on target
    
    digitalWrite(laserPin, LOW);  // laser off
    analogWrite(enPin, 255);      // continue
    delay(1000);
  } else {

    Serial.println(state);
  }
  delay(100);
}
