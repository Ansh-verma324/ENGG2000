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
int motorSpeed = 25;   // 10% of max speed

// ============================================
// Proportional variables
// ============================================
double kp = 1.5;
long targetCount = 0;
int minEffort = 40;

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
// PROPORTIONAL CONTROLLER FUNCTION
// ============================================
long computeP(){

  long error = targetCount - encoderCount;
  double output = kp * error;

  if(output > 255){
    output= 255;
  } else if(output < -255){
    output = -255;
  }

  return (long) output; // return the numbred value as a whole number
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
  
int state = digitalRead(receiverPin); // Read current IR sensor state (LOW = target detected, for VS1838B)

  if (state == LOW) {

    // ============================================
    // TARGET DETECTED — lock this position and hold it
    // ============================================

    Serial.println("IR DETECTED");         // Log that we found the target
    targetCount = encoderCount;            // Remember exactly where we are right now — this becomes our "home" position
    digitalWrite(laserPin, HIGH);          // Turn the laser on

    // Hold this position using P control for a fixed observation window,
    // correcting any drift/displacement caused by momentum

    unsigned long holdStart = millis(); // Record the time we started holding
    while (millis() - holdStart < 5000) { // Keep correcting for 5 seconds total
      long correction = computeP(); // Calculate how hard/which way to correct right now

      if (correction >= 0) {
        digitalWrite(phPin, HIGH);          // Positive correction means drive forward

        int effort; // Declare the effort variable before deciding its value
        if (correction < minEffort && correction > 0) {
          effort = minEffort;   // Correction too weak to move the motor — bump it up to minimum effort
        } else {
          effort = correction;  // Correction is already strong enough — use it as-is
        }

        analogWrite(enPin, effort); // Apply that PWM value to the motor

      } else {
        digitalWrite(phPin, LOW);           // Negative correction means drive reverse

        int effort; // Declare the effort variable before deciding its value
        if (-correction < minEffort) {
          effort = minEffort;   // Correction too weak (in reverse) — bump it up to minimum effort
        } else {
          effort = -correction; // Correction is already strong enough — use its positive magnitude
        }

        analogWrite(enPin, effort); // Apply that PWM value to the motor
      }

      Serial.print("Holding | encoderCount: "); // Debug print: label
      Serial.print(encoderCount);               // Debug print: current position
      Serial.print(" | target: ");              // Debug print: label
      Serial.print(targetCount);                // Debug print: target position
      Serial.print(" | correction: ");          // Debug print: label
      Serial.println(correction);               // Debug print: correction value just applied

      delay(20);  // Small pause before checking/correcting again
    }

    digitalWrite(laserPin, LOW);  // Turn the laser back off after the hold window ends
    analogWrite(enPin, 0);        // Stop the motor completely
    delay(1000);                  // Pause a second before resuming search

  } else {

    digitalWrite(laserPin, LOW); // Off
    digitalWrite(phPin, HIGH);   // Forward direction
    analogWrite(enPin, motorSpeed);
    Serial.println(state);
  }
  delay(100);
}

