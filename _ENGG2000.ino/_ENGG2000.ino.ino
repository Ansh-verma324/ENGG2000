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
const int receiverPin = 8;

// ============================================
// SETUP
// ============================================
void setup() {
  pinMode(enPin, OUTPUT);
  pinMode(phPin, OUTPUT);
  pinMode(laserPin, OUTPUT);
  pinMode(receiverPin, INPUT);

  // Ensure motor is stopped and laser off at startup
  digitalWrite(phPin, HIGH);
  analogWrite(enPin, 0);
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
  
  
  // Ramp up 0 → 255
  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(enPin, speed);
  int state = digitalRead(receiverPin); 
  if (state == LOW) {
  
    // Ramp down 255 → 0
  for (int stopSpeed = speed; stopSpeed >= 0; stopSpeed -= 5) {
    analogWrite(enPin, stopSpeed);
    delay(30);
  }
  analogWrite(enPin, 0);       // Stop motor
  
  digitalWrite(laserPin, HIGH);
  delay(2000);                 // Laser stays ON for 2 seconds
  
  digitalWrite(laserPin, LOW);
  delay(500);                  // Brief pause before reversing
  return;
  }
  delay (30);
  }
      while (true) {

    analogWrite(enPin, 255);

    int state = digitalRead(receiverPin);

    if (state == LOW) {

      Serial.println("IR DETECTED");

      analogWrite(enPin, 0);
      Serial.println("MOTOR STOPPED");

      digitalWrite(laserPin, HIGH);
      Serial.println("LASER ON");

      delay(2000);

      digitalWrite(laserPin, LOW);
      Serial.println("LASER OFF");

      delay(500);

      break; // go back and ramp motor up again
    }

    delay(20);
}
}
