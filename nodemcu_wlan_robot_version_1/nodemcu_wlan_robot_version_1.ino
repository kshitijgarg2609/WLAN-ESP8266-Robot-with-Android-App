#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

int left_speed=D1;
int left_forward=D2;
int left_backward=D3;
int right_speed=D5;
int right_forward=D6;
int right_backward=D7;

boolean tflg=false;
long etime=0;
long timeout=500;

WiFiUDP udp;

void setup()
{
  setUpPins();
  WiFi.mode(WIFI_AP);
  WiFi.softAP("K","123456789");
  udp.begin(4210);
}

void setUpPins()
{
  pinMode(left_speed,OUTPUT);
  pinMode(left_forward,OUTPUT);
  pinMode(left_backward,OUTPUT);
  pinMode(right_speed,OUTPUT);
  pinMode(right_forward,OUTPUT);
  pinMode(right_backward,OUTPUT);
  stopMovement();
}

void loop()
{
  int i=udp.parsePacket();
  if(i>0)
  {
    processPacket(i);
  }
  if(tflg&&((millis()-etime)>=timeout))
  {
    tflg=false;
    stopMovement();
  }
}

void processPacket(int a)
{
  uint8_t rdata[a];
  udp.read(rdata,a);
  String tmp="";
  for(int i=0;i<a;i++)
  {
    tmp+=(char)(rdata[i]);
  }
  cmd(tmp);
}

void cmd(String a)
{
  if(a==NULL)
  {
    return;
  }
  if(a.length()==8 && a.indexOf("op:")==0)
  {
    char md=a.charAt(3);
    int spd=a.substring(4).toInt();
    if(md=='0')
    {
      stopMovement();
      tflg=false;
    }
    else if(md=='1')
    {
      leftMovement();
      speedSet(spd);
      etime=millis();
      tflg=true;
    }
    else if(md=='2')
    {
      rightMovement();
      speedSet(spd);
      etime=millis();
      tflg=true;
    }
    else if(md=='3')
    {
      forwardMovement();
      speedSet(spd);
      etime=millis();
      tflg=true;
    }
    else if(md=='4')
    {
      backwardMovement();
      speedSet(spd);
      etime=millis();
      tflg=true;
    }
  }
}

int speedSet(int a)
{
  if(a>=0 &&a<1024)
  {
    analogWrite(left_speed,a);
    analogWrite(right_speed,a);
  }
}

void stopMovement()
{
  speedSet(0);
  digitalWrite(left_forward,LOW);
  digitalWrite(left_backward,LOW);
  digitalWrite(right_forward,LOW);
  digitalWrite(right_backward,LOW);
}

void leftMovement()
{
  digitalWrite(left_forward,LOW);
  digitalWrite(left_backward,LOW);
  digitalWrite(right_forward,HIGH);
  digitalWrite(right_backward,LOW);
}

void rightMovement()
{
  digitalWrite(left_forward,HIGH);
  digitalWrite(left_backward,LOW);
  digitalWrite(right_forward,LOW);
  digitalWrite(right_backward,LOW);
}

void forwardMovement()
{
  digitalWrite(left_forward,HIGH);
  digitalWrite(left_backward,LOW);
  digitalWrite(right_forward,HIGH);
  digitalWrite(right_backward,LOW);
}

void backwardMovement()
{
  digitalWrite(left_forward,LOW);
  digitalWrite(left_backward,HIGH);
  digitalWrite(right_forward,LOW);
  digitalWrite(right_backward,HIGH);
}
