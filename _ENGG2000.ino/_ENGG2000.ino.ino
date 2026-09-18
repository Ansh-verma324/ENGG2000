/*
 * IR Search + Encoder Position Hold + Laser
 *
 * Behaviour:
 * - No IR -> Motor searches forward
 * - IR detected -> Save current encoder position
 * - Laser ON
 * - P controller holds that position for 2 seconds
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

// IMPORTANT: D2 supports interrupt on Arduino Uno
const int encA = 2;
const int encB = 5;


// ============================================
// VARIABLES
// ============================================

volatile long encoderCount = 0;

long targetCount = 0;

double kp = 1.5;

int minEffort = 40;

int searchSpeed = 65;


// ============================================
// ENCODER INTERRUPT
// ============================================

void encoderISR() {

  if (digitalRead(encB) == HIGH) {

    encoderCount++;

  } else {

    encoderCount--;

  }
}


// ============================================
// PROPORTIONAL CONTROLLER
// ============================================

long computeP() {

  long error = targetCount - encoderCount;

  double output = kp * error;


  // Limit output to valid PWM range

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


  // IR
  pinMode(irPin, INPUT_PULLUP);


  // Laser
  pinMode(laserPin, OUTPUT);


  // Motor
  pinMode(enPin, OUTPUT);
  pinMode(phPin, OUTPUT);


  // Encoder
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);


  // Run encoderISR whenever encoder A changes

  attachInterrupt(
    digitalPinToInterrupt(encA),
    encoderISR,
    CHANGE
  );


  digitalWrite(laserPin, LOW);

  Serial.println("System Ready");
}


// ============================================
// MAIN LOOP
// ============================================

void loop() {

  int state = digitalRead(irPin);


  // ==========================================
  // IR DETECTED
  // ==========================================

  if (state == LOW) {

    Serial.println("IR DETECTED");


    // Save the exact position where target was found

    targetCount = encoderCount;


    // Laser ON

    digitalWrite(laserPin, HIGH);


    // Start 5 second hold period

    unsigned long holdStart = millis();


    while (millis() - holdStart < 2000) {

      long correction = computeP();


      // ======================================
      // Already at target
      // ======================================

      if (correction == 0) {

        analogWrite(enPin, 0);

      }


      // ======================================
      // Need to move forward
      // ======================================

      else if (correction > 0) {

        digitalWrite(phPin, HIGH);

        int effort = correction;


        if (effort < minEffort) {

          effort = minEffort;

        }


        analogWrite(enPin, effort);

      }


      // ======================================
      // Need to move backward
      // ======================================

      else {

        digitalWrite(phPin, LOW);

        int effort = -correction;


        if (effort < minEffort) {

          effort = minEffort;

        }


        analogWrite(enPin, effort);

      }


      // ======================================
      // DEBUG INFORMATION
      // ======================================

      Serial.print("Current: ");
      Serial.print(encoderCount);

      Serial.print(" | Target: ");
      Serial.print(targetCount);

      Serial.print(" | Error: ");
      Serial.print(targetCount - encoderCount);

      Serial.print(" | Correction: ");
      Serial.println(correction);


      delay(20);
    }


    // ========================================
    // Finished firing
    // ========================================

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