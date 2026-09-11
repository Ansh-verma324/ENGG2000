/*
 * Sequential Motor Test with Laser Indicator
 * 
 * Sequence:
 * 1. Motor ramps up/down FORWARD
 * 2. Motor STOPS
 * 3. Laser turns ON
 * 4. Laser turns OFF
 * 5. Motor ramps up/down REVERSE
 * 6. Motor STOPS
 * 7. Laser turns ON
 * 8. Laser turns OFF
 * 9. Repeat
 * 
 * EN = D3, PH = D11, Laser = D7
 * SLEEP = 5V (always awake)
 */

// ============================================
// PIN DEFINITIONS
// ============================================
const int enPin = 3;      // PWM → Speed control
const int phPin = 11;     // Digital → Direction control
const int laserPin = 7;   // Laser indicator

// ============================================
// SETUP
// ============================================
void setup() {
  pinMode(enPin, OUTPUT);
  pinMode(phPin, OUTPUT);
  pinMode(laserPin, OUTPUT);
  
  // Ensure motor is stopped and laser off at startup
  analogWrite(enPin, 0);
  digitalWrite(phPin, LOW);
  digitalWrite(laserPin, LOW);
  
  delay(100);
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  // ============================================
  // STEP 1: MOTOR SPINS FORWARD (ramp up/down)
  // ============================================
  digitalWrite(phPin, HIGH);   // Forward direction
  delay(10);
  
  // Ramp up 0 → 255
  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(enPin, speed);
    delay(30);
  }
  
  // Ramp down 255 → 0
  for (int speed = 255; speed >= 0; speed -= 5) {
    analogWrite(enPin, speed);
    delay(30);
  }
  
  // ============================================
  // STEP 2: MOTOR STOPS
  // ============================================
  analogWrite(enPin, 0);       // Stop motor
  delay(1000);                 // Brief pause after spinning
  
  // ============================================
  // STEP 3: LASER TURNS ON
  // ============================================
  digitalWrite(laserPin, HIGH);
  delay(2000);                 // Laser stays ON for 2 seconds
  
  // ============================================
  // STEP 4: LASER TURNS OFF
  // ============================================
  digitalWrite(laserPin, LOW);
  delay(500);                  // Brief pause before reversing
  
  // ============================================
  // STEP 5: MOTOR SPINS REVERSE (ramp up/down)
  // ============================================
  digitalWrite(phPin, LOW);    // Reverse direction
  delay(10);
  
  // Ramp up 0 → 255
  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(enPin, speed);
    delay(30);
  }
  
  // Ramp down 255 → 0
  for (int speed = 255; speed >= 0; speed -= 5) {
    analogWrite(enPin, speed);
    delay(30);
  }
  
  // ============================================
  // STEP 6: MOTOR STOPS
  // ============================================
  analogWrite(enPin, 0);       // Stop motor
  delay(1000);                 // Brief pause after spinning
  
  // ============================================
  // STEP 7: LASER TURNS ON
  // ============================================
  digitalWrite(laserPin, HIGH);
  delay(2000);                 // Laser stays ON for 2 seconds
  
  // ============================================
  // STEP 8: LASER TURNS OFF
  // ============================================
  digitalWrite(laserPin, LOW);
  delay(500);                  // Brief pause before looping
  
  // ============================================
  // STEP 9: REPEAT
  // ============================================
}
