
#include <Wire.h>
#include "QGPMaker_MotorShield.h"

QGPMaker_MotorShield AFMS = QGPMaker_MotorShield(); //创建驱动器对象
QGPMaker_Servo *Servo0 = AFMS.getServo(3); //获取3号舵机
QGPMaker_Servo *Servo1 = AFMS.getServo(4); //获取3号舵机
QGPMaker_Servo *Servo2 = AFMS.getServo(0); //获取3号舵机
QGPMaker_Servo *Servo3 = AFMS.getServo(7); //获取3号舵机


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("DC Motor test!");

  AFMS.begin(50);
  
}

void loop() {
    //Servo0->writeServo(60);
Servo0->writeServo(40);
Servo1->writeServo(40);
Servo2->writeServo(120);
Servo3->writeServo(0);
  
  
  delay(1000);
  //int deg=Servo3->readDegrees();//读取3#舵机当前角度
  //Serial.println(deg);

}
