
#define LED_Blue 9
#define LED_Yellow 10
#define LED_Green 11

#define Relay 3
#define led_delay 60
#define led_delay2 150

#define Temp_sensor A5
#define TdsSensorPin A6
#define pHsensor  A7

#include <OneWire.h>
#include <EEPROM.h>
#include "GravityTDS.h"

#include <SoftwareSerial.h>
#include <String.h>

#include <DFRobot_SIM808.h>

SoftwareSerial gprsSerial(5,6);
DFRobot_SIM808 sim808(&gprsSerial);

GravityTDS gravityTds;
//-----------------Temp sensor-------------------------//
OneWire  Temperature_Sensor(Temp_sensor);
byte data[9];
byte addr[8]={0x28,0xB3,0x9,0xF1,0x19,0x19,0x1,0x89};
byte type_s=0;
float Temp;

//-----------------TDS sensor-------------------------//
float temperature,tdsValue = 0;

//-----------------PH sensor-------------------------//
float calibration_value = 21.34,volt,ph_act,pH;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;

String mode,longitude, latitude, altitude,time,TTFF,num,speed,course;
int timeout=0;

void setup() {
  Serial.begin(9600);
  gprsSerial.begin(9600);
  delay(1000);

  pinMode(LED_Blue , OUTPUT);
  pinMode(LED_Yellow , OUTPUT);
  pinMode(LED_Green , OUTPUT);
  pinMode(Relay , OUTPUT);

  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();

}



void loop() {
  digitalWrite(LED_Blue, 1);
GET_pos();
digitalWrite(LED_Blue, 0);
digitalWrite(LED_Green, 1);
GET_sensor_data();
digitalWrite(LED_Green, 0);
digitalWrite(LED_Yellow, 1);
GSM_send_Data();
digitalWrite(LED_Yellow, 0);
delay(7199960UL);//(3600*1000*2)-40);
  //Test();

}

//-----------------Temp sensor-------------------------//
 float read_TEMP()
 {
  Temperature_Sensor.search(addr);

  Temperature_Sensor.reset();
  Temperature_Sensor.select(addr);
  Temperature_Sensor.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);

  Temperature_Sensor.reset();
  Temperature_Sensor.select(addr);    
  Temperature_Sensor.write(0xBE);         // Read Scratchpad

   for (int i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = Temperature_Sensor.read();
  }

    int16_t raw = (data[1] << 8) | data[0];
 
  
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  
  return ((float)raw / 16.0);

 }

float Get_pH_Val()
 {
    digitalWrite(Relay, 1);
    delay(3000);
    for(int i=0;i<10;i++)
    {
      buffer_arr[i]=analogRead(pHsensor);
      delay(30);
    }
    for(int i=0;i<9;i++)
    {
      for(int j=i+1;j<10;j++)
      {
        if(buffer_arr[i]>buffer_arr[j])
        {
          temp=buffer_arr[i];
          buffer_arr[i]=buffer_arr[j];
          buffer_arr[j]=temp;
        }
      }
    }
    avgval=0;
    for(int i=2;i<8;i++)
    avgval+=buffer_arr[i];
    digitalWrite(Relay, 0);
    volt=(float)avgval*5.0/1024/6;
    ph_act = -5.70 * volt + calibration_value;
    return ph_act;
 }


void GET_sensor_data(void)
{
  
 temperature=read_TEMP();   //TEMPERATURE
 Serial.print("TEMP=");
 Serial.print(temperature);
 Serial.println("degC");

 gravityTds.setTemperature(temperature);  //TDS
 gravityTds.update(); 
 tdsValue = gravityTds.getTdsValue();
 Serial.print("TDS=");
 Serial.print(tdsValue,0);
 Serial.println("ppm");

 pH=Get_pH_Val();
 Serial.print("pH=");
 Serial.println(pH);

 //GSM_send_Data();
} 

void GSM_send_Data()
{
 if (gprsSerial.available())
    Serial.write(gprsSerial.read());
 
  gprsSerial.println("AT");
  delay(1000);
 
  gprsSerial.println("AT+CPIN?");
  delay(1000);
 
  gprsSerial.println("AT+CREG?");
  delay(1000);
 
  gprsSerial.println("AT+CGATT?");
  delay(1000);
 
  gprsSerial.println("AT+CIPSHUT");
  delay(1000);
 
  gprsSerial.println("AT+CIPSTATUS");
  delay(2000);
 
  gprsSerial.println("AT+CIPMUX=0");
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CSTT=\"bsnlnet.com\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();
  
  String str="GET https://api.thingspeak.com/update?api_key=MSK7BPYW2906M3SK&field1="+String(temperature) +"&field2="+String(tdsValue)+"&field3="+String(pH)+"&field4="+"9.892963699094105"+"&field5="+"76.43897342579177";
  Serial.println(str);
  gprsSerial.println(str);//begin send data to remote server
  
  delay(4000);
  ShowSerialData();
 
  gprsSerial.println((char)26);//sending
  delay(5000);//waitting for reply, important! the time is base on the condition of internet 
  gprsSerial.println();
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
} 
void ShowSerialData()
{
  //while(gprsSerial.available()!=0)
  //Serial.write(gprsSerial.read());
  //delay(5000); 
  
} 
 
void GET_pos()
{
  while(!sim808.getGPS()&&timeout==100)
  {
    timeout++;
    Serial.print(".");
    delay(100);
  }
  timeout=0;
  // if (sim808.getGPS()) 
   {
    Serial.print(sim808.GPSdata.year);
    Serial.print("/");
    Serial.print(sim808.GPSdata.month);
    Serial.print("/");
    Serial.print(sim808.GPSdata.day);
    Serial.print(" ");
    Serial.print(sim808.GPSdata.hour);
    Serial.print(":");
    Serial.print(sim808.GPSdata.minute);
    Serial.print(":");
    Serial.print(sim808.GPSdata.second);
    Serial.print(":");
    Serial.println(sim808.GPSdata.centisecond);
    
    Serial.print("latitude :");
    Serial.println(sim808.GPSdata.lat,6);
    
    sim808.latitudeConverToDMS();
    Serial.print("latitude :");
    Serial.print(sim808.latDMS.degrees);
    Serial.print("\^");
    Serial.print(sim808.latDMS.minutes);
    Serial.print("\'");
    Serial.print(sim808.latDMS.seconeds,6);
    Serial.println("\"");
    Serial.print("longitude :");
    Serial.println(sim808.GPSdata.lon,6);
    sim808.LongitudeConverToDMS();
    Serial.print("longitude :");
    Serial.print(sim808.longDMS.degrees);
    Serial.print("\^");
    Serial.print(sim808.longDMS.minutes);
    Serial.print("\'");
    Serial.print(sim808.longDMS.seconeds,6);
    Serial.println("\"");
    
    Serial.print("speed_kph :");
    Serial.println(sim808.GPSdata.speed_kph);
    Serial.print("heading :");
    Serial.println(sim808.GPSdata.heading);

    //************* Turn off the GPS power ************
    //sim808.detachGPS();
}
}
/*
void Test()
{
  digitalWrite(LED_Blue, 1);
  delay(led_delay);
  digitalWrite(LED_Yellow, 1);
  delay(led_delay);
  digitalWrite(LED_Green, 1);
  delay(led_delay);

  delay(led_delay2);
  digitalWrite(LED_Blue, 0);
  delay(led_delay);
  digitalWrite(LED_Yellow, 0);
  delay(led_delay);
  digitalWrite(LED_Green, 0);
  delay(led_delay);

  digitalWrite(Relay, 1);
  delay(led_delay2);

  digitalWrite(Relay, 0);
  delay(led_delay2);
}
*/