/*
* IR-Triggered Motor Stop with Laser (Noise-Filtered)
* 
* Behaviour:
* - IR detected (D8 = LOW) → Motor STOPS, Laser ON
* - No IR (D8 = HIGH) → Motor spins FORWARD, Laser OFF
* 
* EN = D3, PH = D11, Laser = D7, IR Sensor = D8
*/

// ============================================
// PIN DEFINITIONS
// ============================================
const int irPin = 8; // IR sensor on D8
const int laserPin = 7; // Laser on D7
const int enPin = 3; // Motor speed (PWM) → DRV8874 EN/IN1
const int phPin = 11; // Motor direction → DRV8874 PH/IN2

// ============================================
// SETUP
// ============================================
void setup() {

pinMode(irPin, INPUT_PULLUP);
pinMode(laserPin, OUTPUT);
pinMode(enPin, OUTPUT);
pinMode(phPin, OUTPUT);

}

// ============================================
// MAIN LOOP
// ============================================
void loop() {

digitalWrite(phPin, HIGH);
analogWrite(enPin, 65);
digitalWrite(laserPin, LOW);

int state = digitalRead(irPin);

if (state == LOW) {
analogWrite(enPin, 0); // Stop motor
digitalWrite(laserPin, HIGH); // Laser ON
delay(5000);
} 
else {
// No IR → Motor spins forward, Laser OFF
digitalWrite(phPin, HIGH);
analogWrite(enPin, 65);
digitalWrite(laserPin, LOW);
delay(50); // Ignore sensor during motor startup spike
}
}
