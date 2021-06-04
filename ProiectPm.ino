//Gherman Sebastian-Costin
//https://ocw.cs.pub.ro/courses/pm/prj2021/dbrigalda/502
//Smart Clock

#include <dht11.h>
// Include Wire Library for I2C
#include <Wire.h>
#include <SPI.h>
// Include NewLiquidCrystal Library for I2C
#include <LiquidCrystal_I2C.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <ds3231.h>
 
#define DHT11PIN A0
#define ECHO_PIN 4
#define TRIG_PIN 5
#define WHITE_LED_PIN A2
#define BLUE_LED_PIN A3
#define UP_PIN 6
#define DOWN_PIN 7
#define MENU_PIN 8
#define BUZZER_PIN A1
#define MAX_OPTIONS 6
#define INTERRUPT_PIN 2
 
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 3
 
// We always wait a bit between updates of the display
#define  DELAYTIME  40  // in milliseconds
#define SPEED_TIME  50
#define PAUSE_TIME  1000
 
// Create a new instance of the MD_MAX72XX class:
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
 
// Define LCD pinout
const short  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
 
// Define I2C Address - change if reqiuired
const int i2c_addr = 0x27;
 
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);
 
dht11 DHT11;
bool state = 0;

unsigned char option = 0;
bool upButton; //for forward
bool downButton; //for backward
bool optionButton; //for option

bool nightLights = 0; 
bool noSensors = 0; 
struct ts t; 

void setup()
{
  Wire.begin();
  pinMode(DHT11PIN,INPUT);
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(MENU_PIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT); // Sets the TRIG_PIN as an OUTPUT
  pinMode(ECHO_PIN, INPUT); // Sets the ECHO_PIN as an INPUT
  pinMode(WHITE_LED_PIN,OUTPUT);
  pinMode(BLUE_LED_PIN,OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BLUE_LED_PIN, HIGH);
  
  pinMode(INTERRUPT_PIN,INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN),interrupt_routine,RISING);
  
  DS3231_init(DS3231_CONTROL_INTCN);

  mx.begin();
  
  myDisplay.begin();
  // Set the intensity (brightness) of the display (0-15):
  myDisplay.setIntensity(5);
  // Clear the display:
  LedFunctionTemperatureON();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Pornire Ceas");
  lcd.setCursor(0,1);
  lcd.print("Inteligent!");
  
  tone(BUZZER_PIN, 659, 500);
  delay(100);
  tone(BUZZER_PIN, 523, 500);
  delay(400);
  tone(BUZZER_PIN, 659, 500);
  delay(100);
  tone(BUZZER_PIN, 523, 500);
  delay(400);
  tone(BUZZER_PIN, 784, 500);
  
  scrollText("Salut !");
  delay(2000);
  mx.control(MD_MAX72XX::INTENSITY, 0);
  spiral();
  delay(3000);
  mx.control(MD_MAX72XX::INTENSITY, 15);
  
  lcd.clear();
 
  myDisplay.displayClear();
 
  myDisplay.setTextAlignment(PA_CENTER);
  digitalWrite(BLUE_LED_PIN, LOW);
}
 
void scrollText(const char *p)
{
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts
 
  mx.clear();
 
  while (*p != '\0')
  {
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
 
    for (uint8_t i=0; i<=charWidth; i++)  // allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      if (i < charWidth)
        mx.setColumn(0, cBuf[i]);
      delay(DELAYTIME);
    }
  }
}

void bounce()
// Animation of a bouncing ball
{
  const int minC = 0;
  const int maxC = mx.getColumnCount()-1;
  const int minR = 0;
  const int maxR = ROW_SIZE-1;

  int  nCounter = 0;

  int  r = 0, c = 2;
  int8_t dR = 1, dC = 1;  // delta row and column

  mx.clear();

  while (nCounter++ < 200)
  {
    mx.setPoint(r, c, false);
    r += dR;
    c += dC;
    mx.setPoint(r, c, true);
    delay(DELAYTIME/4);
    if ((r == minR) || (r == maxR))
      dR = -dR;
    if ((c == minC) || (c == maxC))
      dC = -dC;
  }
}

void newHour(){
  //can be changed for hours
  if(t.sec == 0 || t.sec == 1) bounce();
}

void spiral()
// setPoint() used to draw a spiral across the whole display
{
  int  rmin = 0, rmax = ROW_SIZE-1;
  int  cmin = 0, cmax = (COL_SIZE*MAX_DEVICES)-1;
 
  mx.clear();
  while ((rmax > rmin) && (cmax > cmin))
  {
    // do row
    for (int i=cmin; i<=cmax; i++)
    {
      mx.setPoint(rmin, i, true);
      delay(DELAYTIME/MAX_DEVICES);
    }
    rmin++;
 
    // do column
    for (uint8_t i=rmin; i<=rmax; i++)
    {
      mx.setPoint(i, cmax, true);
      delay(DELAYTIME/MAX_DEVICES);
    }
    cmax--;
 
    // do row
    for (int i=cmax; i>=cmin; i--)
    {
      mx.setPoint(rmax, i, true);
      delay(DELAYTIME/MAX_DEVICES);
    }
    rmax--;
 
    // do column
    for (uint8_t i=rmax; i>=rmin; i--)
    {
      mx.setPoint(i, cmin, true);
      delay(DELAYTIME/MAX_DEVICES);
    }
    cmin++;
  }
}
 
void getCurrentTemperature(){
  DHT11.read(DHT11PIN);
  tone(BUZZER_PIN, 523, 500);
  delay(300);
  tone(BUZZER_PIN, 659, 500);
  delay(300);
  tone(BUZZER_PIN, 784, 500);
  lcd.setCursor(0,0);
  lcd.print("Umiditate: ");
  lcd.print((float)DHT11.humidity, 2);
  
  lcd.setCursor(0,1);
  lcd.print("Temp (C): ");
  lcd.print((float)DHT11.temperature, 2);
  
  delay(5000);
 
  lcd.clear();

}

void getUltrasonicDistance(){
  unsigned int duration;
  unsigned short distance;
  // Clears the TRIG_PIN condition
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the TRIG_PIN HIGH (ACTIVE) for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    
  if (distance < 6 && distance > 0){
    lcd.clear();
    LedFunctionTemperatureON();
    for(unsigned char i = 0; i < 10; i++){  
      lcd.print(".");
      delay(100);
    }
    
    getCurrentTemperature();    
    digitalWrite(BLUE_LED_PIN, HIGH);
    digitalWrite(WHITE_LED_PIN, LOW);
  }
  delay(10);
}
 
void displayClock(){
  DS3231_get(&t);
  newHour();
  char timeToPrint[10] = {};
  char hoursPrint, minutesPrint;
  if(t.hour < 10) hoursPrint ='0';
  else hoursPrint = ' ';
  if(t.min < 10) minutesPrint = '0';
  else minutesPrint = ' ';
  sprintf(timeToPrint,"%c%d:%c%d",hoursPrint, t.hour, minutesPrint, t.min);
  
  myDisplay.print(timeToPrint);
}

void displayDate(){ 
  DS3231_get(&t);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(t.mday);
  lcd.print("/");
  lcd.print(t.mon);
  lcd.print("/");
  lcd.print(t.year);  
}

void LedFunctionClock(){
  digitalWrite(WHITE_LED_PIN, !digitalRead(WHITE_LED_PIN));
  digitalWrite(BLUE_LED_PIN, !digitalRead(BLUE_LED_PIN));
}

void LedFunctionTemperatureON(){
  digitalWrite(WHITE_LED_PIN, HIGH); 
  digitalWrite(BLUE_LED_PIN, HIGH);
}

void displaySetHours(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Seteaza Ora: ");
  if(upButton){
    if(t.hour == 23)
      t.hour = 0;
    else t.hour++;
  }
  if(downButton){
    if(t.hour == 0)
      t.hour = 23;
    else t.hour--;
  }
  t.sec = 0;
  lcd.setCursor(0,1);
  lcd.print(t.hour);
  DS3231_set(t); 
}

void displaySetMinutes(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Seteaza Minut: ");
  if(upButton){
    if(t.min == 59)
      t.min = 0;
    else t.min++;
  }
  if(downButton){
    if(t.min == 0)
      t.min = 59;
    else t.min--;
  }
  t.sec = 0;
  lcd.setCursor(0,1);
  lcd.print(t.min);
  DS3231_set(t); 
}

void displaySetDays(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Seteaza Ziua: ");
  if(upButton){
    if(t.mon % 2 == 0){
      //max days 30
      if(t.mday == 31)
        t.mday = 1;
      else t.mday ++;     
    }
    else {
      if(t.mday == 32)
        t.mday = 1;
      else t.mday ++;      
    } 
  }
  
  if(downButton){
    if(t.mon % 2 == 0){
      //max days 30
      if(t.mday == 1)
        t.mday = 30;
      else t.mday--;
    }
    else {
      if(t.mday == 1)
        t.mday = 31;
      else t.mday--;
    }
  }
  lcd.setCursor(0,1);
  lcd.print(t.mday);
  DS3231_set(t); 
}

void displaySetMonths(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Seteaza Luna: ");
  if(upButton){
    if(t.mon == 13)
      t.mon = 1;
    else t.mon++;
  }
  
  
  if(downButton){
     if(t.mon == 13)
      t.mon = 1;
     else t.mon--;
  }
  lcd.setCursor(0,1);
  lcd.print(t.mon);
  DS3231_set(t); 
}
void displaySetYears(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Seteaza Anul: ");
  if(upButton){
    t.year++;
  }
  if(downButton){
     t.year--;
  }
  lcd.setCursor(0,1);
  lcd.print(t.year);
  DS3231_set(t); 
}
 
void buttonFunction(){  
  delay(500);
  upButton = !digitalRead(UP_PIN);
  downButton = !digitalRead(DOWN_PIN);
  optionButton = !digitalRead(MENU_PIN); 
  if(optionButton)
    option++; 

  if(upButton && option == 0){
    nightLights = !nightLights;
    
    if(nightLights){
      lcd.clear();
      lcd.print("Night Lights ON");
      delay(1000);
      lcd.clear();
  }
    else{
      lcd.clear();
      lcd.print("Night Lights OFF");
      delay(1000);
      lcd.clear();
    }
  }
  
  if(downButton && option == 0){
    noSensors = !noSensors;
    if(!noSensors){
      lcd.clear();
      lcd.print("Sensors ON");
      delay(1000);
      lcd.clear();
    }
    else{
      lcd.clear();
      lcd.print("Sensors OFF");
      delay(1000);
      lcd.clear();
    }
  }
  
  if(option == MAX_OPTIONS){
    lcd.clear();
    lcd.print("Ceasul s-a setat");
    delay(1000);
    lcd.clear();
    option = 0;
  }
    
}
void menuFunction(){
   switch(option){
    case 1:
      displaySetHours();
      break;
    case 2:
      displaySetMinutes();
      break;
    case 3:
      displaySetDays();
      break;
    case 4:
      displaySetMonths();
      break;
    case 5:
      displaySetYears();
      break;
    default:
      displayClock();
      break;
  }
}

void motionDetected(){
  if(state == 1 && option == 0){
    lcd.clear();
    lcd.setCursor(0,0);
    
    lcd.print("Hai noroc");
    delay(3000);
    lcd.clear();
    lcd.print("Apropie mana ");
    lcd.setCursor(0,1);
    lcd.print("de senzor ");
    delay(5000);
    lcd.clear();
    state = 0;
  }
}

void loop()
{ 
  if(nightLights){
    mx.control(MD_MAX72XX::INTENSITY, 1);  
    digitalWrite(BLUE_LED_PIN, 0);
    digitalWrite(WHITE_LED_PIN, 0);
  }
  else{
    //if night lights was on previously turn one led
    if(digitalRead(BLUE_LED_PIN) == 0 && digitalRead(WHITE_LED_PIN) == 0 )
      digitalWrite(BLUE_LED_PIN, 1);
    LedFunctionClock();
    delay(500);
    mx.control(MD_MAX72XX::INTENSITY, 10);    
  }
    
  if(!noSensors){
    motionDetected();
    getUltrasonicDistance();
  }
  
  buttonFunction();  
  displayDate();
  menuFunction();
}
 
void interrupt_routine(){
  state = 1;
}
