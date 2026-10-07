//Trial 10: Merge of Trial 6-7 (encoder + P control + switch) and Trial 8 (gyro)
// Correction during hold is now based on gyro-measured angle displacement

#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

/*
 * IR Search + Gyro-Based Angle Hold + Laser + Switch
 *
 * Behaviour:
 * - Switch OFF -> everything stops
 * - Switch ON, No IR -> Motor searches forward
 * - Switch ON, IR detected -> Save current gyro angle as target
 * - Laser ON
 * - P controller corrects based on GYRO angle error for 2 seconds
 * - Laser OFF
 * - Resume search
 */

// ============================================
// PIN DEFINITIONS
// ============================================

const int irPin = 8;

const int laserPin = 7;

const int enPin = 3;
const int phPin = 11;

const int switchPin = 10; // Change this to correct pin

// ============================================
// GYRO VARIABLES
// ============================================

float gyroZ = 0;          // current angular velocity, deg/s
float gyroAngle = 0;      // integrated angle estimate, degrees
float targetAngle = 0;    // angle we want to hold, degrees
unsigned long lastGyroTime = 0;

// ============================================
// PROPORTIONAL CONTROLLER VARIABLES
// ============================================

double kp = 1.5;          // will likely need retuning for angle-based error — start lower and test
int minEffort = 40;
int searchSpeed = 65;

// ============================================
// UPDATE GYRO ANGLE (integrates velocity into position)
// ============================================
void updateGyroAngle() {
  mpu.update();
  unsigned long now = millis();
  float dt = (now - lastGyroTime) / 1000.0;
  if (dt <= 0) dt = 0.001; // guard against divide-by-zero on first call
  lastGyroTime = now;

  gyroZ = mpu.getGyroZ();
  gyroAngle += gyroZ * dt; // accumulate velocity into an angle estimate
}

// ============================================
// PROPORTIONAL CONTROLLER (angle-based)
// ============================================
long computeP() {

  float error = targetAngle - gyroAngle; // error in degrees now, not encoder ticks
  double output = kp * error;

  if (output > 255) {
    output = 255;
  }
  else if (output < -255) {
    output = -255;
  }

  return (long)output;
}

// ============================================
// SETUP
// ============================================

void setup() {
  Serial.begin(9600);
  Wire.begin();

  mpu.begin();
  Serial.println("Calculating gyro offsets, do not move MPU6050...");
  mpu.calcOffsets();
  Serial.println("Done!");

  // IR
  pinMode(irPin, INPUT_PULLUP);

  // Laser
  pinMode(laserPin, OUTPUT);

  // Motor
  pinMode(enPin, OUTPUT);
  pinMode(phPin, OUTPUT);

  // Switch
  pinMode(switchPin, INPUT_PULLUP);

  digitalWrite(laserPin, LOW);

  lastGyroTime = millis();

  Serial.println("System Ready");
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {

  bool systemOn = (digitalRead(switchPin) == LOW);

  if (!systemOn) {
    analogWrite(enPin, 0);
    digitalWrite(laserPin, LOW);
    return;
  }

  updateGyroAngle(); // keep the angle estimate current every loop pass

  int state = digitalRead(irPin);

  // ==========================================
  // IR DETECTED
  // ==========================================

  if (state == LOW) {
    Serial.println("IR DETECTED");

    // Save the current gyro-estimated angle as our hold target
    targetAngle = gyroAngle;

    digitalWrite(laserPin, HIGH);

    unsigned long holdStart = millis();

    while (millis() - holdStart < 2000) {
      updateGyroAngle(); // refresh angle estimate during the hold too

      long correction = computeP();

      if (correction == 0) {
        analogWrite(enPin, 0);
      }
      else if (correction > 0) {
        digitalWrite(phPin, HIGH);
        int effort = correction;
        if (effort < minEffort) {
          effort = minEffort;
        }
        analogWrite(enPin, effort);
      }
      else {
        digitalWrite(phPin, LOW);
        int effort = -correction;
        if (effort < minEffort) {
          effort = minEffort;
        }
        analogWrite(enPin, effort);
      }

      Serial.print("GyroAngle: ");
      Serial.print(gyroAngle);
      Serial.print(" | Target: ");
      Serial.print(targetAngle);
      Serial.print(" | Error: ");
      Serial.print(targetAngle - gyroAngle);
      Serial.print(" | Correction: ");
      Serial.println(correction);

      delay(20);
    }

    digitalWrite(laserPin, LOW);
    analogWrite(enPin, 0);
    delay(1000);
  }

  // ==========================================
  // NO IR -> SEARCH
  // ==========================================

  else {
    digitalWrite(laserPin, LOW);
    digitalWrite(phPin, HIGH);
    analogWrite(enPin, searchSpeed);
    delay(50);
  }
}