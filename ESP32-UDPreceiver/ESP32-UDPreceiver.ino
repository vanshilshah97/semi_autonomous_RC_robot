/*Divyanshu Sahu, Vanshil Shah
 * receives packets from UDP-TCPsender.c (plus any other broadcasters)
 * 
 */
#include <WiFi.h>
#include <WiFiUdp.h>

#define LEDC_CHANNEL  0//
#define LEDC_RESOLUTION_BITS 10 // 10 bit
#define LEDC_RESOLUTION ((1<<LEDC_RESOLUTION_BITS)-1)
#define LEDC_FREQ_HZ 5000
#define LED_PIN 23

const char* ssid     = "TP-Link_05AF"; //set the appropriate router ssid and password
const char* password = "47543454";

WiFiUDP UDPTestServer; //initialize the server and the IP address
unsigned int UDPPort = 2808;
IPAddress myIPaddress(192, 168, 0, 8);

const int UDP_PACKET_SIZE = 48; // can be up to 65535
byte packetBuffer[UDP_PACKET_SIZE+1];

void ledcAnalogWrite(uint8_t channel, uint32_t value,uint32_t valueMax = 255){//255 is default
  float ratio = ((float)value/(float)valueMax);
  float duty = LEDC_RESOLUTION*ratio;
  ledcWrite(channel,duty); // write duty to LEDC
}

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");  Serial.println(ssid);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.config(myIPaddress, IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, password);

  UDPTestServer.begin(UDPPort); // strange bug needs to come after WiFi.begin but before connect
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");  
 packetBuffer[UDP_PACKET_SIZE] = 0; // null terminate buffer
 // packetBuffer = 0;
  pinMode(23,OUTPUT); //PWM 1 and 2
  pinMode(19,OUTPUT); //MOTOR1DIR
  pinMode(22,OUTPUT); //MOTOR2DIR


 ledcSetup(LEDC_CHANNEL, LEDC_FREQ_HZ,LEDC_RESOLUTION_BITS);
 ledcAttachPin(LED_PIN, LEDC_CHANNEL);
}

void loop() {
   handleCanMsg();
   delay(10);
   
}

void handleCanMsg() {
const int UDP_PACKET_SIZE = 14; // can be up to 65535
uint8_t packetBuffer[UDP_PACKET_SIZE];
int cb = UDPTestServer.parsePacket(); // if there is no message cb=0
Serial.println(cb);
if (cb) {
  int x,y;
  packetBuffer[cb]=0; // null terminate string
  UDPTestServer.read(packetBuffer, UDP_PACKET_SIZE);
  x = atoi((char *)packetBuffer+2); // #,####,#### 2nd indexed char
  y = atoi((char *)packetBuffer+7); // #,####,#### 7th indexed char
  Serial.print("From Can ");
  Serial.println((char *)packetBuffer);
  Serial.println(x);
  Serial.println(y);
  //change the motor rotation direction as per the button value recieved
  if(y == 1){
    digitalWrite(19, HIGH);
    digitalWrite(22, HIGH);
    }
  if ( y == 2 )
  {
    digitalWrite(19, LOW);
    digitalWrite(22, LOW);
    }
  if ( y == 3 )
  {
    digitalWrite(19, HIGH);
    digitalWrite(22, LOW);
  }
  if ( y == 4 )
  {
    digitalWrite(19, LOW);
    digitalWrite(22, HIGH);
    }  
 
   if(y ==0)
   {
    ledcAnalogWrite(LEDC_CHANNEL, 0); //set the motor speed
    }
    else
    {
      ledcAnalogWrite(LEDC_CHANNEL, 200); //set the motor speed
      }
  }
}
