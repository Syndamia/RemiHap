#include <Servo.h> 
// Note: servo.h library disables analogWrite on pins 9 and 10

#define INPUT_PIN A0
int potValue; // Potentiometer value, will be the input value from the main controller

Servo servos[5];
const int SERVO_PINS[] = {3, 5, 6, 9, 10};

void zeroOutServos() {
  for (int i = 0; i < sizeof(servos)/sizeof(*servos); i++) { // would've used range-based for loop, but its not supported on C++98
    servos[i].write(0);
  }
}

void setup() {
  pinMode(INPUT_PIN, INPUT);
  
  for(int i = 0; i < sizeof(servos)/sizeof(*servos); i++) {
    pinMode(SERVO_PINS[i], OUTPUT);
    servos[i].attach(SERVO_PINS[i]);
  }
  
  delay(1500);
  zeroOutServos();
}

void dispensePill(int servoIndex) {
  servos[servoIndex].write((servos[servoIndex].read() == 180)?0:180);
  delay(1500);
}

void loop() { // For demo purposes, continuously rotates selected servo
  potValue = analogRead(INPUT_PIN) >> 2;
  dispensePill(potValue * 5 / 255);
  delay(100);
}
