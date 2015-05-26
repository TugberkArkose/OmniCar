#include <TimerOne.h>
#include <LSM303.h>
#include <Wire.h>
#include <EEPROM.h>
byte password;
LSM303 compass;
int motor1_dir = 8;
int motor1_pwm = 9;
int motor2_dir = 10;
int motor2_pwm = 11;
int motor3_dir = 12;
int motor3_pwm = 3;
int panicBttnPin = 0;
char incoming;
const int pingPin1 = 4;
const int pingPin2 = 5;
const int pingPin3 = 7;
const int buzzerPin = 6;
unsigned int duration, inches;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
const long COMPASS_DELAY = 3000;
const long SENSOR_DELAY = 200;
boolean isNorthLocked = false;
boolean isSouthWest = false;
boolean isSouthEast = false;
boolean isPanic = false;
void setup() {
  Timer1.initialize(2000);    // initialize timer1
  Timer1.attachInterrupt(readIncoming);    //attaches readIncoming() as a timer overflow interrupt
  
  pinMode(motor1_dir, OUTPUT);
  pinMode(motor1_pwm, OUTPUT);
  pinMode(motor2_dir, OUTPUT);
  pinMode(motor2_pwm, OUTPUT);
  pinMode(motor3_dir, OUTPUT);
  pinMode(motor3_pwm, OUTPUT);
  pinMode(panicBttnPin, INPUT);
  digitalWrite(motor1_dir, HIGH);
  digitalWrite(motor2_dir, HIGH);
  digitalWrite(motor3_dir, HIGH);
  analogWrite(motor1_pwm, 0);
  analogWrite(motor2_pwm, 0);
  analogWrite(motor3_pwm, 0);
  
  pinMode(buzzerPin,OUTPUT);
  
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  compass.m_min = (LSM303::vector<int16_t>) {-32767, -32767, -32767};
  compass.m_max = (LSM303::vector<int16_t>) {+32767, +32767, +32767};
  
  attachInterrupt(panicBttnPin, panicButtonPressed, FALLING);
}
void loop() {
  if(!isPanic){
    unsigned long currentMillis = millis();
   
    if(currentMillis - previousMillis1 >= SENSOR_DELAY){
      detectObstacleNorth();
      detectObstacleSouthWest();
      detectObstacleSouthEast();
      previousMillis1 = currentMillis;
    }
    
    Serial.flush();
    if(currentMillis - previousMillis2 >= COMPASS_DELAY) {
      Serial.flush();
      compass.read();
      int heading = (int)compass.heading();
      Serial.println(heading);
      Serial.flush();
      previousMillis2 = currentMillis;
    }
  }
}
void readIncoming(){
  if(!isPanic){
    if(Serial.available() > 0) {
        incoming = Serial.read();
        Serial.flush();
        if(incoming == 'n'){
          analogWrite(motor1_pwm, 0);
          analogWrite(motor2_pwm, 0);
          analogWrite(motor3_pwm, 0);
        }
        else if(!isNorthLocked && incoming == 'w'){
          digitalWrite(motor1_dir, HIGH);
          analogWrite(motor1_pwm, 255);
          digitalWrite(motor2_dir, HIGH);
          analogWrite(motor2_pwm, 255);
          analogWrite(motor3_pwm, 0);
        }  
        else if(!isSouthWest && !isSouthEast && incoming == 's'){
          digitalWrite(motor1_dir, LOW);
          analogWrite(motor1_pwm, 255);
          digitalWrite(motor2_dir, LOW);
          analogWrite(motor2_pwm, 255);
          analogWrite(motor3_pwm, 0);
        }
        else if(!isSouthWest && incoming == 'a'){
          digitalWrite(motor1_dir, HIGH);
          analogWrite(motor1_pwm, 150);
          digitalWrite(motor2_dir, LOW);
          analogWrite(motor2_pwm, 100);
          digitalWrite(motor3_dir, HIGH);
          analogWrite(motor3_pwm, 255);
        }
        else if(!isSouthEast && incoming == 'd'){
          digitalWrite(motor1_dir, LOW);
          analogWrite(motor1_pwm, 150);
          digitalWrite(motor2_dir, HIGH);
          analogWrite(motor2_pwm, 100);
          digitalWrite(motor3_dir, LOW);
          analogWrite(motor3_pwm, 255);
        }
        else if(!isNorthLocked && incoming == 'q'){
          digitalWrite(motor1_dir, HIGH);
          analogWrite(motor1_pwm, 255);
          digitalWrite(motor2_dir, HIGH);
          analogWrite(motor2_pwm, 60);
          //digitalWrite(motor3_dir, LOW);
          analogWrite(motor3_pwm, 0);
        }    
        else if(!isNorthLocked && incoming == 'e'){
          digitalWrite(motor1_dir, HIGH);
          analogWrite(motor1_pwm, 60);
          digitalWrite(motor2_dir, HIGH);
          analogWrite(motor2_pwm, 255);
          //digitalWrite(motor3_dir, LOW);
          analogWrite(motor3_pwm, 0);
        }
        else if(!isSouthWest && incoming == 'z'){
          digitalWrite(motor1_dir, LOW);
          analogWrite(motor1_pwm, 255);
          digitalWrite(motor2_dir, LOW);
          analogWrite(motor2_pwm, 60);
          //digitalWrite(motor3_dir, LOW);
          analogWrite(motor3_pwm, 0);
        }
        else if(!isSouthEast && incoming == 'x'){
          digitalWrite(motor1_dir, LOW);
          analogWrite(motor1_pwm, 60);
          digitalWrite(motor2_dir, LOW);
          analogWrite(motor2_pwm, 255);
          //digitalWrite(motor3_dir, LOW);
          analogWrite(motor3_pwm, 0);
        }
      }  
   }
}
void detectObstacleNorth(){  
    pinMode(pingPin1, OUTPUT);       // Set pin to OUTPUT         
    digitalWrite(pingPin1, LOW);        // Ensure pin is low
    delayMicroseconds(2);
    digitalWrite(pingPin1, HIGH);       // Start ranging
    delayMicroseconds(5);              //   with 5 microsecond burst
    digitalWrite(pingPin1, LOW);        // End ranging
    pinMode(pingPin1, INPUT);           // Set pin to INPUT
    duration = pulseIn(pingPin1, HIGH,20000); // Read echo pulse
    inches = duration / 74 / 2;        // Convert to inches  
  
    if(inches<6 && inches>0){
      isNorthLocked = true;
      digitalWrite(motor1_dir, LOW);
      analogWrite(motor1_pwm, 255);
      digitalWrite(motor2_dir, LOW);
      analogWrite(motor2_pwm, 255);
      analogWrite(motor1_pwm, 0);
      analogWrite(motor2_pwm, 0);
      analogWrite(motor3_pwm, 0);
      analogWrite(buzzerPin, 20); // beep
      delay(50);
      analogWrite(buzzerPin,0);
      Serial.flush();
    }
    else{
      isNorthLocked = false;
    }
 }
 
 void detectObstacleSouthWest(){
   pinMode(pingPin2, OUTPUT);       // Set pin to OUTPUT         
    digitalWrite(pingPin2, LOW);        // Ensure pin is low
    delayMicroseconds(2);
    digitalWrite(pingPin2, HIGH);       // Start ranging
    delayMicroseconds(5);              //   with 5 microsecond burst
    digitalWrite(pingPin2, LOW);        // End ranging
    pinMode(pingPin2, INPUT);           // Set pin to INPUT
    duration = pulseIn(pingPin2, HIGH,20000); // Read echo pulse
    inches = duration / 74 / 2;        // Convert to inches  
  
    if(inches<6 && inches>0){
        isSouthWest = true;
        digitalWrite(motor1_dir, HIGH);
        analogWrite(motor1_pwm, 255);
        digitalWrite(motor2_dir, HIGH);
        analogWrite(motor2_pwm, 255);
        digitalWrite(motor3_dir, LOW);
        analogWrite(motor3_pwm, 255);
        analogWrite(motor1_pwm, 0);
        analogWrite(motor2_pwm, 0);
        analogWrite(motor3_pwm, 0);
        analogWrite(buzzerPin, 20); // beep
        delay(50);
        analogWrite(buzzerPin,0);
        Serial.flush();
      }
      else{
      isSouthWest = false;
    }
 }
 
  void detectObstacleSouthEast(){
   pinMode(pingPin3, OUTPUT);       // Set pin to OUTPUT         
    digitalWrite(pingPin3, LOW);        // Ensure pin is low
    delayMicroseconds(2);
    digitalWrite(pingPin3, HIGH);       // Start ranging
    delayMicroseconds(5);              //   with 5 microsecond burst
    digitalWrite(pingPin3, LOW);        // End ranging
    pinMode(pingPin3, INPUT);           // Set pin to INPUT
    duration = pulseIn(pingPin3, HIGH,20000); // Read echo pulse
    inches = duration / 74 / 2;        // Convert to inches  
  
    if(inches<6 && inches>0){
        isSouthEast = true;
        digitalWrite(motor1_dir, HIGH);
        analogWrite(motor1_pwm, 255);
        digitalWrite(motor2_dir, HIGH);
        analogWrite(motor2_pwm, 255);
        digitalWrite(motor3_dir, HIGH);
        analogWrite(motor3_pwm, 255);
        analogWrite(motor3_pwm, 0);
        analogWrite(motor1_pwm, 0);
        analogWrite(motor2_pwm, 0);
        analogWrite(buzzerPin, 20); // beep
        delay(50);
        analogWrite(buzzerPin,0);
        Serial.flush();
      }
      else{
      isSouthEast = false;
    }
 }
 
 void panicButtonPressed(){
      isPanic = true;
      analogWrite(motor1_pwm, 0);
      analogWrite(motor2_pwm, 0);
      analogWrite(motor3_pwm, 0);
 }
