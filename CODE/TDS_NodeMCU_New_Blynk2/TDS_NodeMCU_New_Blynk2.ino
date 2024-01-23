#define BLYNK_DEVICE_NAME "Water Quality Monitoring"
#define BLYNK_TEMPLATE_ID "TMPLeOu-f4fW"
#define BLYNK_TEMPLATE_NAME "IOT based Water Quality Monitoring"
#define BLYNK_AUTH_TOKEN "0iNAwT66kNR0VN6CqPPnWI9--cJlqLE4"
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "POCO X3";
char pass[] = "rahul1234";
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"
int LED=D6;
int DSPIN = D5; //Temperature Sensor
int TDS_Sensor = A0;
float Aref = 3.3;
float ec_Val = 0;
unsigned int tds_value = 0;
float ecCal = 1;
WidgetLED led(V6);
void sendSensor()
{
  double wTemp = TempRead()* 0.0625;// conversion accuracy is 0.0625 / LSB
  float V_level= Aref / 1024.0;
  float rawEc = analogRead(TDS_Sensor) * V_level;  // Raw  data of EC
  float T_Cof = 1.0 + 0.02 * (wTemp - 25.0);  // Temperature Coefficient
  
  ec_Val = (rawEc / T_Cof) * ecCal;// temperature and calibration compensation
  tds_value = (133.42 * pow(ec_Val, 3) - 255.86 * ec_Val * ec_Val + 857.39 * ec_Val) * 0.5; 
  double Far= (((wTemp *9)/5) + 32); // Temp in Far*
  
  Serial.print("TDS:"); Serial.println(tds_value);
  Serial.print("EC:"); Serial.println(ec_Val, 2);
  Serial.print("Temperature (oC):"); Serial.println(wTemp,2);   
  Serial.print("Temperature (oF):"); Serial.println(Far,2);
  Serial.print("");
 
  Blynk.virtualWrite(V0,wTemp);
  Blynk.virtualWrite(V1,Far);
  Blynk.virtualWrite(V2,tds_value);
  Blynk.virtualWrite(V3,ec_Val);
  // Determine water quality based on TDS value
  String quality;
  String pure;
  if(tds_value>=50&&tds_value<=150){
    quality="Excellent";
    pure="OFF";
    led.off();
    digitalWrite(LED,LOW);
  } else if(tds_value > 150 && tds_value<=250){
    quality="Good";
    pure="OFF";
    led.off();
    digitalWrite(LED,LOW);
  } else if(tds_value>250&&tds_value<=300){
    quality="Fair";
    pure="ON";
    led.on();
    digitalWrite(LED,HIGH);
  } else if(tds_value>300&&tds_value<=500){
    quality="Poor";
    pure="ON";
    led.on();
    digitalWrite(LED,HIGH); // Turn on led
    Blynk.logEvent("high_tds_alert");
  } 
   else if(tds_value>=500){
    quality="Unsafe";
    pure="ON";
    digitalWrite(LED,HIGH); // Turn on led
    Blynk.logEvent("high_tds_alert");
    led.on();
  }
  else{
    quality="Unknown";
    pure="OFF";
    digitalWrite(LED,LOW);
    led.off();
  }
  
  Serial.print("Water Quality: "); Serial.println(quality);
  Serial.print("Water Purifier: "); Serial.println(pure);
  // Display TDS value and water quality on Blynk virtual LCD widget v04 pin
  Blynk.virtualWrite(V4,quality);
  Blynk.virtualWrite(V5,pure);

}
void setup() 
{
  Serial.begin(9600); // Dubugging on hardware Serial 0
  BlynkEdgent.begin();
  pinMode(LED,OUTPUT); // Set led pin as output
  pinMode(TDS_Sensor,INPUT);
  delay(2000); 
}
 
void loop() 
{
  BlynkEdgent.run();
  sendSensor();
  delay(1000);
}
