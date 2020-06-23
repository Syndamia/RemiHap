#include <Servo.h>
// Note: the library disables analogWrite on pins 9 and 10

Servo servo_1;
Servo servo_2;
Servo servo_3;
Servo servo_4;
Servo servo_5;

// Potentiometer value, will be the input value from the main controller
int potValue;

void zeroOutServos() {
  servo_1.write(0);
  servo_2.write(0);
  servo_3.write(0);
  servo_4.write(0);
  servo_5.write(0);
}

#define INPUT_PIN A0
#define SERVO_1_PIN 3
#define SERVO_2_PIN 5
#define SERVO_3_PIN 6
#define SERVO_4_PIN 9
#define SERVO_5_PIN 10

void setup()
{
  pinMode(INPUT_PIN, INPUT);
  
  pinMode(SERVO_1_PIN, OUTPUT);
  servo_1.attach(SERVO_1_PIN);
  pinMode(SERVO_2_PIN, OUTPUT);
  servo_2.attach(SERVO_2_PIN);
  pinMode(SERVO_3_PIN, OUTPUT);
  servo_3.attach(SERVO_3_PIN);
  pinMode(SERVO_4_PIN, OUTPUT);
  servo_4.attach(SERVO_4_PIN);
  pinMode(SERVO_5_PIN, OUTPUT);
  servo_5.attach(SERVO_5_PIN);
  
  delay(2000);
  zeroOutServos();
}

void loop()
{
  potValue = analogRead(INPUT_PIN) >> 2;
  // For demo purposes, continuously rotates servo
  dispensePill((potValue * 5 / 255) + 1);
  delay(100);
}

void dispensePill(int servoIndex) {
  switch(servoIndex) {
    case 1: servo_1.write((servo_1.read() == 180)?0:180); break;
    case 2: servo_2.write((servo_2.read() == 180)?0:180); break;
    case 3: servo_3.write((servo_3.read() == 180)?0:180); break;
    case 4: servo_4.write((servo_4.read() == 180)?0:180); break;
    case 5: servo_5.write((servo_5.read() == 180)?0:180); break;
  }
  delay(1500);
}
