
//Libraries
#include <Keypad.h>
#include<dht.h>
#include<LiquidCrystal.h>

LiquidCrystal lcd(43,45,47,49,51,53);
dht DHT;

//Input Pins
#define dhtinput A0
#define phinput A1

//Output Pins
int hydropump=10;
int phpump=11;
int nutripump=12;
int glight=13;
int flight=14;
int fan=15;

//Variables
unsigned long int avgValue; //Average pH
float b;
int phreg[10],temp;
int phValue;

int mode;
int temps;
int humi;

char customKey;

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
 
  pinMode(hydropump,OUTPUT);
  pinMode(phpump,OUTPUT);
  pinMode(nutripump,OUTPUT);
  pinMode(glight,OUTPUT);
  pinMode(flight,OUTPUT);
  pinMode(fan,OUTPUT);
  
  //Serial
  Serial.begin(9600);     
  Serial.println("DEVELOPER MODE");
//LCD
  lcd.begin(16,2);
  
  lcd.setCursor(1,0);
  lcd.print("HYDROPONINATOR");
  lcd.setCursor(6,1);
  lcd.print("By Doof");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("SELECT MODE");
  lcd.setCursor(4,1);
  lcd.println("A B C D     ");
  delay(2000);
}

void loop() 
{
  phbalance();
  customKey = customKeypad.getKey();
  if (customKey=='A') // Automatic Mode
  {
    mode=0;
    lcd.clear();
    lcd.print("DEFAULT MODE");
    delay(1000);
    automode();
    
  }
  if (customKey=='B') // Manual Mode
  {
    mode=1;
    lcd.clear();
    lcd.print("MANUAL MODE");
    delay(1000);
    manualmode();
    
  }

  if (customKey=='C') // Sensor Display
  {
    mode=0;
    lcd.clear();
    lcd.print("SENSOR DISPLAY");
    lcd.clear();
    phsens();
    lcd.setCursor(0,0);
    lcd.print("pH= ");
    lcd.setCursor(6,0);
    lcd.print(phValue);
    dhtsens();
    lcd.setCursor(1,1);
    lcd.print("T= ");
    lcd.setCursor(3,1);
    lcd.print(temps);
    lcd.setCursor(5,1);
    lcd.print(" C ");
    lcd.setCursor(9,1);
    lcd.print("H= ");
    lcd.setCursor(11,1);
    lcd.print(humi);
    lcd.setCursor(13,1);
    lcd.print(" % ");
    
    
  }
  
  if ((customKey=='1') && (mode==1))
 {
     digitalWrite(hydropump,HIGH);
     Serial.print("PUMP");
 }
  if ((customKey=='4') && (mode==1))
  {
    digitalWrite(hydropump,LOW);
    Serial.print("OFF");
  }
  if ((customKey=='2') && (mode==1))
  {
    digitalWrite(nutripump,HIGH);
  }
  if ((customKey=='5') && (mode==1))
  {
    digitalWrite(nutripump,LOW);
  }
  if ((customKey=='3') && (mode==1))
  {
    digitalWrite(phpump,HIGH);
  }
  if ((customKey=='6') && (mode==1))
  {
    digitalWrite(phpump,LOW);
  }
  if ((customKey=='7') && (mode==1))
  {
    digitalWrite(fan,HIGH);
  }
  if ((customKey=='*') && (mode==1))
  {
    digitalWrite(fan,LOW);
  }
  if ((customKey=='8') && (mode==1))
  {
    digitalWrite(flight,HIGH);
  }
  if ((customKey=='0') && (mode==1))
  {
    digitalWrite(flight,LOW);
  }
  if ((customKey=='9') && (mode==1))
  {
    digitalWrite(glight,HIGH);
  }
  if ((customKey=='#') && (mode==1))
  {
    digitalWrite(glight,LOW);
  }

}

void manualmode()
{
 lcd.clear();
 lcd.setCursor(1,0);
 lcd.print("Please Wait");
 lcd.setCursor(3,1);
 lcd.print("Initiating...");
 delay(1000);
 lcd.clear(); 
 lcd.setCursor(1,0);
 lcd.print("MANUAL MODE ON");
 lcd.setCursor(3,1);
 lcd.print("PRESS A KEY");
}  


void automode()
{
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Please Wait");
  lcd.setCursor(3,1);
  lcd.print("Initiating...");
  delay(1000);
  lcd.clear();    
  lcd.setCursor(3,0);
  lcd.print("MOTOR ON");
  digitalWrite(hydropump,LOW);
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print("LIGHTS ON");
  digitalWrite(glight,LOW);
  digitalWrite(flight,LOW);
  delay(2000);
  /*lcd.setCursor(10,1);
  lcd.print("FAN ON");
  delay(1000);*/
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("AUTO MODE");
  dhtsens();
  lcd.setCursor(0,1);
  lcd.print("T-");
  lcd.setCursor(4,1);
  lcd.print(temps);
  lcd.setCursor(6,1);
  lcd.print("C");
  lcd.setCursor(8,1);
  lcd.print("H-");
  lcd.setCursor(11,1);
  lcd.print(humi);
  lcd.setCursor(14,1);
  lcd.print("%");
  phsens();
  lcd.setCursor(10,0);
  lcd.print("pH-");
  lcd.setCursor(13,0);
  lcd.print(phValue);
  phbalance();
}

void dhtsens() //DHT11 Sensor
{   
    DHT.read11(dhtinput);
    humi=DHT.humidity;
    temps=DHT.temperature;
  
}

void phsens() //pH Sensor
{
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    phreg[i]=analogRead(phinput);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(phreg[i]>phreg[j])
      {
        temp=phreg[i];
        phreg[i]=phreg[j];
        phreg[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //average value of 6 center sample
    avgValue+=phreg[i];
    phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;  //convert the millivolt into pH value
}

void phbalance()
{
  phsens();
  if(phValue>7.5)
  {
  digitalWrite(phpump,HIGH);
  delay(3000);
  return;
  }
  else
  {
    digitalWrite(phpump,LOW);
    return;
  }
}
