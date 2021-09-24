#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include "motor.h"


#define NUM_ANGLES 9 // num of angles to scan
#define ANGLES (int[]) { 30, 60, 70, 80, 90, 100, 110, 120, 150 } //angles to scan

// Ultrasonic Module pins
#define TRIG_PIN 13 // 10 microsecond high pulse causes chirp , wait 50 us
#define ECHO_PIN 12 // Width of high pulse indicates distance

// NeoPixel setup
#define LED_PIN A0 //pin for the NeoPixel LED strand
#define LED_COUNT NUM_ANGLES //length of the NeoPixel LED strand
#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)

// Servo motor that aims ultrasonic sensor .
#define SERVO_PIN 11 // PWM output for hobby servo

// Motor control pins : L298N H bridge
#define EN_A_PIN 6 // Left motor PWM speed control
#define IN_1_PIN 7 // Left motor Direction 1
#define IN_2_PIN 5 // Left motor Direction 2
#define IN_3_PIN 4 // Right motor Direction 1
#define IN_4_PIN 2 // Right motor Direction 2
#define EN_B_PIN 3 // Right motor PWM speed control

Servo servo;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB);

Motor RIGHT_MOTOR = { IN_1_PIN, IN_2_PIN, EN_A_PIN };
Motor LEFT_MOTOR =  { IN_3_PIN, IN_4_PIN, EN_B_PIN };

// main
unsigned int currentAngleIndex;
unsigned int currentDistance;

void setup() { 

  Serial.begin(9600);
  Serial.print("In Setup!\n");
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN , LOW);
  pinMode(EN_A_PIN, OUTPUT);
  pinMode(IN_1_PIN, OUTPUT);
  pinMode(IN_2_PIN, OUTPUT);
  pinMode(IN_3_PIN, OUTPUT);
  pinMode(IN_4_PIN, OUTPUT);
  pinMode(EN_B_PIN, OUTPUT);
  servo.attach(SERVO_PIN);
  servo.write (90);
  motorStop();

  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);

  //testMotors();
  delay(1000);
  
  for (int i = 0; i < NUM_ANGLES; i++) {
    readDistance(ANGLES[i]);
    setAngleWhite(i);
    delay(1000);
  }
  
  currentAngleIndex = 0;
  currentDistance = readDistance(ANGLES[currentAngleIndex]);

  motorGoForward(FULL_SPEED);

  
}


void loop() {
    if (currentDistance < 350) {
      Serial.print("Obstacle detected! stopping\n");
      setAngleRed(currentAngleIndex);
      motorStop();
      delay(200);
      
      unsigned int angle = ANGLES[currentAngleIndex];
      
      if (angle > 90 || (angle == 90 && random(2))) {
        motorTurnRight(FULL_SPEED);
      } else {
        motorTurnLeft(FULL_SPEED);
      }
      
      delay(250);
      motorGoBackward(HALF_SPEED);
      delay(750);
    } else {
      setAngleGreen(currentAngleIndex);
    }
    
    motorGoForward(FULL_SPEED);
    
    currentAngleIndex++;
    currentAngleIndex %= NUM_ANGLES;
    currentDistance = readDistance(ANGLES[currentAngleIndex]);
    delay(100 );
}

int setAngleWhite(int index) {
  setPixelColor(index, strip.Color(255, 255, 255));
}

int setAngleRed(int index) {
  setPixelColor(index, strip.Color(255, 0, 0));
}

int setAngleGreen(int index) {
  setPixelColor(index, strip.Color(0, 255, 0));
}

int setPixelColor(int index, uint32_t color) {
  uint32_t off = strip.Color(0, 0, 0);
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, i == index ? color : off);         
    strip.show();            
  } 
          
}

int getSpeedValue(enum Speed speed) {
  switch (speed) {
    case FULL_SPEED: return 200;
    case HALF_SPEED: return 127;
    case QUARTER_SPEED: return 63;
  }
  return 0;
}

void motor(Motor motor, enum Direction direction, enum Speed speed)
{
  digitalWrite(motor.forwardPin , direction == FORWARD ? HIGH : LOW );
  digitalWrite(motor.backwardsPin, direction == BACKWARD ? HIGH : LOW );
  analogWrite(motor.speedPin, getSpeedValue(speed) );
}

void motorGoForward(enum Speed speed) {
  Serial.print("Going forward\n");
  motor(LEFT_MOTOR, FORWARD, speed);
  motor(RIGHT_MOTOR, FORWARD, speed);
}

void motorGoBackward(enum Speed speed) {
  Serial.print("Going backward\n");
  motor(LEFT_MOTOR, BACKWARD, speed);
  motor(RIGHT_MOTOR, BACKWARD, speed);
}


void motorStop() {
  Serial.print("Stopping\n");
  motor(LEFT_MOTOR, FORWARD, STOP);
  motor(RIGHT_MOTOR, FORWARD, STOP);
}

void motorTurnRight(enum Speed speed) {
  Serial.print("Going right\n");
  motor(LEFT_MOTOR, FORWARD, speed);
  motor(RIGHT_MOTOR, BACKWARD, speed);
}


void motorTurnLeft(enum Speed speed) {
  Serial.print("Going left\n");
  motor(LEFT_MOTOR, BACKWARD, speed);
  motor(RIGHT_MOTOR, FORWARD, speed);
}


unsigned int readDistance() {
  digitalWrite ( TRIG_PIN , HIGH ); 
  delayMicroseconds (10);
  digitalWrite ( TRIG_PIN , LOW );
  unsigned long period = pulseIn ( ECHO_PIN, HIGH ); 
  unsigned int distance = period * 343 / 2000;
  Serial.print("Read distance of ");
  Serial.print(distance);
  Serial.print("\n"); 
  return distance;
}

unsigned int readDistance(unsigned int angle) {
  servo.write(angle);
  Serial.print("Setting angle to "); 
  Serial.print(angle);
  Serial.print(" --  ");
  return readDistance();
}

void testMotors()
{
  Serial.print("Starting test\n");
  delay(1000);

  motorGoForward(FULL_SPEED);
  delay(2500);

  motorTurnRight(HALF_SPEED);
  delay(500);
  motorGoForward(FULL_SPEED);
  delay(2500);

  motorStop();
  delay(1000);

  motorGoBackward(FULL_SPEED);
  delay(2500);
  motorStop();
  delay(1000);
  Serial.print("Ending test\n");
}
