

/*
 * MEAM510 Lab 4 demo
 * March 2021
 * 
 * Mark Yim
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 */
#include <WiFi.h>
#include "html510.h"
#include "joyJS.h"
#include "tankJS.h"
#include <WiFiUdp.h>
#include "Adafruit_VL53L0X.h"
#include "vive510.h"
int flag=0;
/*********UDP***********/
char s[13];

WiFiUDP UDPTestServer;
unsigned int UDPPort = 2510; // port for cans is 1510, port for robots is 2510
IPAddress ipTarget(192, 168, 1, 255); // 255 is a broadcast address to everyone at 192.168.1.xxx

void fncUdpSend(char *datastr, int len)
{
  UDPTestServer.beginPacket(ipTarget, UDPPort);
  UDPTestServer.write((uint8_t *)datastr, len);
  UDPTestServer.endPacket();
}

// CAN CODE
int pose_x  = 1;
int pose_y = 1;
WiFiUDP canUDPServer;
//IPAddress myIPaddress(192, 168, 1, 9); // change to your IP

void handleCanMsg() {
  const int UDP_PACKET_SIZE = 14; // can be up to 65535
  uint8_t packetBuffer[UDP_PACKET_SIZE];
  
  int cb = canUDPServer.parsePacket();
  if (cb) {
    int x,y;
    packetBuffer[cb]=0; // null terminate string
    canUDPServer.read(packetBuffer, UDP_PACKET_SIZE);
    
    x = atoi((char *)packetBuffer+2);
    y = atoi((char *)packetBuffer+7);

    //Serial.print("From Can ");
    int id;
    id=atoi((char *)packetBuffer);
    //Serial.println("iDDD");
    //Serial.println(id);
    if(id==7)
    {
      if(x>0 && y>0)
      {
        pose_x=x;
        pose_y=y; 
      }
}
      
    //Serial.println((char *)packetBuffer);
    Serial.println("GOALX");
    Serial.println(pose_x);
   Serial.println("GOALY");
//     
    Serial.println(pose_y);
    }
}

// CAN CODE
/*********WALL FOLLOW***********/

#include <NewPing.h>

#define SONAR_NUM 1      // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.


NewPing sonar[SONAR_NUM] = {   // Sensor object array.
  NewPing(13, 9, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping. 
};

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;



/*********WALL FOLLOW***********/
/*********VIVEEE***********/
#define SIGNALPIN1 18 // pin receiving signal from Vive circuit AGE WALA
#define SIGNALPIN2 22 // pin receiving signal from Vive circuit PICHE WALA
Vive510 vive1(SIGNALPIN1);
Vive510 vive2(SIGNALPIN2);
int x_1, y_1, x_2, y_2;
/*********VIVEEE***********/
int range1 = 1;
int range2 = 1;


long time1;
long time2;
long time3;


WiFiServer server(80);
const char* ssid     = "TP-Link_E0C8";
const char* password = "52665134";
//const char* ssid     = "TP-Link_05AF";
//const char* password = "47543454";
const char *body ;

/********************/
/* HTML510  web   */
void handleFavicon(){
  sendplain(""); // acknowledge
}

void handleRoot() {
  sendhtml(body);
}

//void handleSwitch() { // Switch between JOYSTICK and TANK mode
//  String s="";
////  static int toggle=0;
////  if (toggle) body = joybody;
////  else body = tankbody;
////  toggle = !toggle;
////switch_state = switch_state+1;
////if(switch_state ==3)
////{
////  switch_state = 0;
////} 
  
//  sendplain(s); //acknowledge
//}
#define LEDC_CHANNEL  0//
#define LEDC_RESOLUTION_BITS 10 // 10 bit
#define LEDC_RESOLUTION ((1<<LEDC_RESOLUTION_BITS)-1)
#define LEDC_FREQ_HZ 5000
#define LED_PIN 26
#define LED_PIN_2 25
#define DIR_PIN1 27 //
#define DIR_PIN2 14 //
#define LEDC_CHANNEL_2  1//

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {            
  uint32_t duty =  LEDC_RESOLUTION * min(value, valueMax) / valueMax;   
  ledcWrite(channel, duty);  // write duty to LEDC 
}

/************************/
/* joystick mode  code  */



/*********************/
/* tank mode  code  */
int leftstate, rightstate;
int leftarm, rightarm;
long lastLeverMs;
int switch_state = 10;
void handleManual() {
  // do something? 
  Serial.println("Manual Mode");
  switch_state = 1;
  sendplain(""); //acknowledge
}


void handleWall() {
  // do something? 
  Serial.println("Wall Follow");
  switch_state = 2;
  sendplain(""); //acknowledge
}


void handleBeacon() {
  // do something? 
  Serial.println("Beacon");\
  switch_state = 3;
  sendplain(""); //acknowledge
}

void handleUDP() {
  // do something? 
  Serial.println("UDP");
  switch_state = 4;
  sendplain(""); //acknowledge
}



void handleLever() {

  if(switch_state ==1)
  {

  leftarm = getVal();
  rightarm = getVal();
  leftstate = getVal();
  rightstate = getVal();
  String s = String(leftarm) + "," + String(rightarm) + "," +
             String(leftstate) + "," + String(rightstate);
 
  time1=millis();
  flag=1;
  lastLeverMs = millis(); //timestamp command
  sendplain(s);
  Serial.printf("received %d %d %d %d \n",leftarm, rightarm, leftstate, rightstate); // move bot  or something
  
/*********UDP***********/
  //int val = map(analogRead(33), 0, 4096, 0, 255);
  int val = 255;
  int k =0;
  if (leftstate>0)      k=2; //backwards
  else if (leftstate<0) k=1; //forwards
  if (rightstate>0)      k=3; //right
  else if (rightstate<0) k=4; //left
  Serial.print("k");
  Serial.println(k);

  //change the motor rotation direction as per the button value recieved
  if(k == 1){
    digitalWrite(DIR_PIN1, HIGH);
    digitalWrite(DIR_PIN2, HIGH);
          ledcAnalogWrite(LEDC_CHANNEL, 200); //set the motor speed
      ledcAnalogWrite(LEDC_CHANNEL_2, 200); //set the motor speed
    }
  if ( k == 2 )
  {
    digitalWrite(DIR_PIN1, LOW);
    digitalWrite(DIR_PIN2, LOW);
          ledcAnalogWrite(LEDC_CHANNEL, 200); //set the motor speed
      ledcAnalogWrite(LEDC_CHANNEL_2, 200); //set the motor speed

    }
  if ( k == 3 )
  {
    digitalWrite(DIR_PIN1, LOW);
    digitalWrite(DIR_PIN2, HIGH);
          ledcAnalogWrite(LEDC_CHANNEL, 200); //set the motor speed
      ledcAnalogWrite(LEDC_CHANNEL_2, 200); //set the motor speed

    
  }
  if ( k == 4 )
  {
    digitalWrite(DIR_PIN1, HIGH);
    digitalWrite(DIR_PIN2, LOW);
          ledcAnalogWrite(LEDC_CHANNEL, 200); //set the motor speed
      ledcAnalogWrite(LEDC_CHANNEL_2, 200); //set the motor speed

  }  
 
  if(k ==0)
  {
    ledcAnalogWrite(LEDC_CHANNEL, 0); //set the motor speed
    ledcAnalogWrite(LEDC_CHANNEL_2, 0); //set the motor speed
  }
  } 
}

void setup() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 1, 88), 
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));
  
  
  while(WiFi.status()!= WL_CONNECTED ) { 
    delay(500); Serial.print("."); 
  }
  Serial.println("WiFi connected"); 
  Serial.printf("Use this URL http://%s/\n",WiFi.localIP().toString().c_str());
  server.begin();                  //Start server
  UDPTestServer.begin(UDPPort); 
  canUDPServer.begin(1510); // can port 1510
  pinMode(19,INPUT_PULLUP);
  pinMode(21,INPUT_PULLUP);


//  UDPTestServer.begin(udpTargetPort);
  
 // WALL FOLLOW

  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));

  
 // WALL FOLLOW ENDS
  // VIVEEEEEEEEEEEEEEEEEEEE
  vive1.begin();
  vive2.begin();
  Serial.println("Vive trackers started");
 // VIVEE

 // Servo initialization
  pinMode(LED_PIN,OUTPUT); //PWM 1 and 2
  pinMode(DIR_PIN1,OUTPUT); //MOTOR1DIR
  pinMode(DIR_PIN2,OUTPUT); //MOTOR2DIR
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ_HZ,LEDC_RESOLUTION_BITS);
  ledcSetup(LEDC_CHANNEL_2, LEDC_FREQ_HZ,LEDC_RESOLUTION_BITS);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL);
  ledcAnalogWrite(LEDC_CHANNEL, 0); //set the motor speed
  ledcAttachPin(LED_PIN_2, LEDC_CHANNEL_2);
  ledcAnalogWrite(LEDC_CHANNEL_2, 0); //set the motor speed


//IR SENSE

  pinMode(32, INPUT); // left
  pinMode(33, INPUT);  //right
  pinMode(10,OUTPUT);
 // HTML510 initialization
 // attachHandler("/joy?val=",handleJoy);
//  attachHandler("/armup",handleArmup);
//  attachHandler("/armdown",handleArmdown);
 // attachHandler("/switchmode",handleSwitch);
  attachHandler("/manualcontrolmode",handleManual);
  attachHandler("/wallfollowingmode",handleWall);
  attachHandler("/beacontrackingmode",handleBeacon);
  attachHandler("/UDPcanfollowmode",handleUDP);
  attachHandler("/lever?val=",handleLever);
  body = tankbody;
  
  attachHandler("/favicon.ico",handleFavicon);
  attachHandler("/ ",handleRoot);
}

int id =2;
void loop()
{ flag=0;
  
  if(digitalRead(19)==1 && digitalRead(21)==1)
  { id=2;

  Serial.println("id is 1");
    }
    if(digitalRead(19)==1 && digitalRead(21)==0)
  { id=1;
    Serial.println("id is 2");
    }
    if(digitalRead(19)==0 && digitalRead(21)==1)
  { id=3;
    Serial.println("id is 3");
    }
    
    if(digitalRead(19)==0 && digitalRead(21)==0)
  { id=4;
    Serial.println("id is 4");
    }
  
  static long lastWebCheck = millis();
//  static long lastServoUpdate = millis();
  uint32_t ms;
  //serve(server,body); 
  ms = millis();
  if (ms-lastWebCheck > 2){ 
 serve(server,body);    
lastWebCheck = ms;
 }


// VIVE STARTSSSSSS

static long lastWebCheck2 = millis();

  if (vive1.status() == VIVE_LOCKEDON) {
    //Serial.printf("X1 %d, Y1 %d\n",vive1.xCoord(),vive1.yCoord());
    x_1 = vive1.xCoord();
    y_1 = vive1.yCoord();
   
   
   sprintf(s,"%1d:%4d,%4d",id, x_1, y_1); 
  if (ms-lastWebCheck2 > 2000){ 
 fncUdpSend(s,13);
 lastWebCheck2 = ms;
 }
  
  
  //Serial.printf("sending data: %s",s); 
  
  }
  else
  { 
     vive1.sync(15); // try to resync (nonblocking);
     //Serial.println("Vive1");
  }
  if (vive2.status() == VIVE_LOCKEDON) {
    //Serial.printf("X2 %d, Y2 %d\n",vive2.xCoord(),vive2.yCoord());
    x_2 = vive2.xCoord();
    y_2 = vive2.yCoord(); 
  sprintf(s,"%1d:%4d,%4d",id, x_2, y_2); 
// fncUdpSend(s,13);
  if (ms-lastWebCheck2 > 2000){ 
 fncUdpSend(s,13);
 lastWebCheck2 = ms;
 }
  Serial.printf("sending data: %s",s);   
  
  }
  else
  { 
     vive2.sync(15); // try to resync (nonblocking);
     //Serial.println("Vive2");
  }
  if(vive1.status() == VIVE_LOCKEDON && vive2.status() == VIVE_LOCKEDON)
  {
//    x_vive=vive2.xCoord();
//    y_vive=vive2.yCoord();
  
  }

 //VIVE ENDS



  if(switch_state == 2)
  {
    wall_follow();
    //Serial.println("STATE CHANGES");
  }
  if(switch_state == 3)
  {
    irsense();
    //Serial.println("STATE CHANGES");
  }
  if(switch_state == 4)
  {
    canfollow();
    //Serial.println("STATE CHANGES"); UDP daal de
  }
}
// CAN FOLLOWING BEGINS

int dis1 = 1;
int dis2 = 1;
void canfollow()
{
    x_1 = vive1.xCoord();
    y_1 = vive1.yCoord();
    
    Serial.println("x_1");
    Serial.println(x_1);
    Serial.println("y_1");
    Serial.println(y_1);
    //Serial.printf("X1 %d, Y1 %d\n",vive1.xCoord(),vive1.yCoord());

    x_2 = vive2.xCoord();
    y_2 = vive2.yCoord();
    Serial.println("x_2");
    Serial.println(x_2);
    Serial.println("y_2");
    Serial.println(y_2);
    //Serial.println(count);

    dis1 = sqrt((pose_x - x_1)*(pose_x - x_1) + (pose_y- y_1)*(pose_y- y_1));
    dis2 = sqrt((pose_x - x_2)*(pose_x - x_2) + (pose_y- y_2)*(pose_y- y_2));
  
  follow(pose_x, pose_y);
  delay(200); 
}


void follow(float pos_x, float pos_y)
{
   int pwm = 180;
   float diff = abs(dis2-dis1);
    Serial.println("dis1");
    Serial.println(dis1);
    Serial.println("dis2");
    Serial.println(dis2);   
    Serial.println("diff");
    Serial.println(diff);

   
   if((abs(dis2-dis1) <340&& abs(dis2-dis1)>260 && dis2>dis1))
   {
    robot_control(1, 200);
     delay(200);
     Serial.println("FORWARD0");
   }
   else
   {
     robot_control(4, 190);
     delay(100);
     Serial.println("RIGHT");
     robot_control(4, 0);
     delay(100);
   }
}

// CAN FOLLOW ENDS

void wall_follow()
{
  Serial.println("SONARR");
  Serial.println(sonar[0].ping_cm());
  range1 = sonar[0].ping_cm()*10;
  
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance for TOF(mm): "); Serial.println(measure.RangeMilliMeter);
    range2 = measure.RangeMilliMeter;
  } else {
    Serial.println(" out of range ");
  }



  if(range2> 280)
  {
   if(range1<180)
   {
    digitalWrite(DIR_PIN1, HIGH); //move forward
    digitalWrite(DIR_PIN2, HIGH);
    ledcAnalogWrite(LEDC_CHANNEL, 160); //set the motor speed
    ledcAnalogWrite(LEDC_CHANNEL_2, 180); //set the motor speed
    Serial.println("RIGHT");
    delay(100);
   }
   else
   {
    digitalWrite(DIR_PIN1, HIGH); //move forward
    digitalWrite(DIR_PIN2, HIGH);
    ledcAnalogWrite(LEDC_CHANNEL, 180); //set the motor speed
    ledcAnalogWrite(LEDC_CHANNEL_2, 160); //set the motor speed
    Serial.println("LEFT");
    delay(100);    
   }
   }
   else
   {
    if(range2<280)
    {
      digitalWrite(DIR_PIN1, LOW);
      digitalWrite(DIR_PIN2, HIGH);
      ledcAnalogWrite(LEDC_CHANNEL, 170); //set the motor speed
      ledcAnalogWrite(LEDC_CHANNEL_2, 170); //set the motor speed
      Serial.println("TURN");
      delay(100);    
    }
   }
}
//ir sense begins
float high1 =5;
float low1 = 5;
float high2 = 5;
float low2 = 5;
int safe_1 = 0;
int safe_2 = 0;
void irsense()
{
  high1 = pulseIn(32, HIGH);
  low1 = pulseIn(32, LOW);
  float total1 = high1 + low1; // Time period of the pulse in microseconds
  float frequency1 = 1000000.0 / total1; // Frequency in Hertz (Hz) 
  if(frequency1>550 && frequency1 <950)
  {
    safe_1 = 1;
  
  }

  else {     safe_1 = 0;

}

  high2 = pulseIn(33, HIGH);
  low2 = pulseIn(33, LOW);
  float total2 = high2 + low2; // Time period of the pulse in microseconds
  float frequency2 = 1000000.0 / total2; // Frequency in Hertz (Hz)
  if(frequency2>550 && frequency2 <950)
  {
    safe_2 = 1;
  }
  else {    safe_2 = 0;

}
  Serial.println("frequency1");
  Serial.println(frequency1);

  Serial.println("frequency2");
  Serial.println(frequency2);
  if(safe_1 == 1 && safe_2== 1)
  {
    robot_control(1, 180);
    Serial.println("FORWARD");
    delay(500);
  }
  if(safe_1 == 0 && safe_2== 1)
  {
    robot_control(4, 170);
    Serial.println("RIGHT");
  }
  if(safe_1 == 1 && safe_2== 0)
  {
    robot_control(3, 170);
    Serial.println("LEFT");
  }
    if(safe_1 == 0 && safe_2== 0)
  {
    robot_control(3, 170);
    Serial.println("RANDOM");
  }
  delay(100);  
}

void robot_control(int k, int pwm)
{
    //change the motor rotation direction as per the button value recieved
  if(k == 1){ //forward
    digitalWrite(DIR_PIN1, HIGH);
    digitalWrite(DIR_PIN2, HIGH);
    }
  if ( k == 2 ) //backwards
  {
    digitalWrite(DIR_PIN1, LOW);
    digitalWrite(DIR_PIN2, LOW);
    }
  if ( k == 3 ) //left
  {
    digitalWrite(DIR_PIN1, HIGH);
    digitalWrite(DIR_PIN2, LOW);
  }
  if ( k == 4 ) //right 
  {
    digitalWrite(DIR_PIN1, LOW);
    digitalWrite(DIR_PIN2, HIGH);
  }
  if(k ==0)
  {
    ledcAnalogWrite(LEDC_CHANNEL, 0); //set the motor speed
    ledcAnalogWrite(LEDC_CHANNEL_2, 0); //set the motor speed
  }
  else
  {
    ledcAnalogWrite(LEDC_CHANNEL, pwm); //set the motor speed
    ledcAnalogWrite(LEDC_CHANNEL_2, pwm); //set the motor speed
  }  
  
}

//IR SENSE ENDSSSS
