//Trial 4: 

/*
 * Motor Control: Forward → Stop → Reverse → Stop
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

const int laserPin = 7; //Pin for the laser 

/*const int irLedPin = 6; //IR emitter led pin 
const int irRecieverPin = 7; //IR reciever output pin
*/

// ============================================
// VARIABLES
// ============================================
volatile long encoderCount = 0;
int motorSpeed = 65;   // 25% of max speed

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

// Sends a short burst of ~38kHz IR by toggling the LED manually
/*void sendIRBurst() {
  for (int i = 0; i < 200; i++) {
    digitalWrite(irLedPin, HIGH); // LED on
    delayMicroseconds(13);        // ~38kHz half-period (1/38000/2 ≈ 13.16us)
    digitalWrite(irLedPin, LOW);  // LED off
    delayMicroseconds(13);
  }
}

// Sends the burst, then checks if the receiver picked it up
bool checkIRDetected() {
  sendIRBurst();
  int state = digitalRead(irReceiverPin);
  return (state == LOW);                  // when signal is detected
}*/

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
  
  // Attach interrupt - encoderISR is now declared
  attachInterrupt(digitalPinToInterrupt(encA), encoderISR, CHANGE);
  
  Serial.println("Motor Control Ready!");
  Serial.println("Sequence: Forward → Stop → Reverse → Stop");
  Serial.println("------------------------------------------");
  delay(1000);
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  // ============================================
  // 1. MOVE FORWARD
  // ============================================
  Serial.println("▶ FORWARD");
  digitalWrite(laserPin, LOW); // Off
  digitalWrite(phPin, HIGH);   // Forward direction
  analogWrite(enPin, motorSpeed);
  encoderCount = 0;             // Reset encoder count
  delay(5000);                  // Run for 5 seconds
  
  // Show encoder counts
  Serial.print("  Encoder Pulses: ");
  Serial.println(encoderCount);
  Serial.println();
  
  // ============================================
  // 2. STOP
  // ============================================
  
  Serial.println("■ STOP");
  analogWrite(enPin, 0);        // Brake
  digitalWrite(laserPin, HIGH); // On
  delay(2000);                  // Stop for 2 seconds
  Serial.println();
  
  // ============================================
  // 3. MOVE REVERSE
  // ============================================
  Serial.println("◀ REVERSE");
  digitalWrite(laserPin, LOW);  // Off
  digitalWrite(phPin, LOW);     // Reverse direction
  analogWrite(enPin, motorSpeed);
  encoderCount = 0;             // Reset encoder count
  delay(5000);                  // Run for 5 seconds
  
  // Show encoder counts
  Serial.print("  Encoder Pulses: ");
  Serial.println(encoderCount);
  Serial.println();
  
  // ============================================
  // 4. STOP
  // ============================================
  Serial.println("■ STOP");
  analogWrite(enPin, 0);        // Brake
  digitalWrite(laserPin, HIGH); // On
  delay(2000);                  // Stop for 2 seconds
  Serial.println();
  Serial.println("========== Loop Repeating ==========");
  Serial.println();
}
