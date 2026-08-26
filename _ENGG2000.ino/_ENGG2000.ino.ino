//Template 1: 

/*
 * Motor Control with IR Obstacle Detection
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

const int laserPin = 5; // Pin for the laser 

const int irLedPin = 6;      // IR emitter LED
const int receiverPin = 7;   // IR receiver output pin

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
// IR FUNCTIONS
// ============================================
void sendIRBurst() {
  for (int i = 0; i < 200; i++) {
    digitalWrite(irLedPin, HIGH);
    delayMicroseconds(13);
    digitalWrite(irLedPin, LOW);
    delayMicroseconds(13);
  }
}

bool checkIRDetected() {
  sendIRBurst();
  int state = digitalRead(receiverPin);
  return (state == LOW); // obstacle detected
}

// ============================================
// SETUP
// ============================================
void setup() {
  Serial.begin(9600);
  
  pinMode(enPin, OUTPUT);
  pinMode(phPin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  
  digitalWrite(sleepPin, HIGH);
  delay(10);
  
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);

  pinMode(laserPin, OUTPUT);

  pinMode(irLedPin, OUTPUT);
  pinMode(receiverPin, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(encA), encoderISR, CHANGE);
  
  Serial.println("Motor Control Ready!");
  delay(1000);
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  if (checkIRDetected()) {
    // Obstacle found - stop motor, laser on
    Serial.println("IR DETECTED - STOPPING");
    analogWrite(enPin, 0);        // Stop motor
    digitalWrite(laserPin, HIGH); // Laser on
  } else {
    // Clear - keep moving, laser off
    digitalWrite(laserPin, LOW);
    digitalWrite(phPin, HIGH);    // Forward direction
    analogWrite(enPin, motorSpeed);
  }

  delay(100);
}