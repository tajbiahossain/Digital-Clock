#include "SevSeg.h"
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <LiquidCrystal.h>
#include "DHT.h"

int contrast = 75;
#define DHTPIN 13

SevSeg Display;
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char temperature[] = "Temp = 00.0 C  ";
char humidity[]    = "RH   = 00.0 %  ";
const int ledPin =  A0;
unsigned long number;
unsigned long currentMillis;
unsigned long Hour;
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 500;

void setup()
{
  pinMode(ledPin, OUTPUT);
  analogWrite(6, contrast);
  lcd.begin(16, 2);
  dht.begin();
  Serial.begin(9600);
  byte numDigits = 6;
  byte digitPins[] = {0, 1, 2, 3, 4, 5};
  byte segmentPins[] = {22, 23, 24, 25, 26, 27, 28, 29};
  bool resistorsOnSegments = true;
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_CATHODE;
  bool updateWithDelays = false;
  bool leadingZeros = false;
  bool disableDecPoint = false;
  Display.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  Display.setBrightness(100);
}

void loop()
{
  delay(1000); // wait 1s between readings
  byte RH = dht.readHumidity(); // Read humidity
  byte Temp = dht.readTemperature(); //Read temperature in degree Celsius

  // Check if any reads failed and exit early (to try again)
  if (isnan(RH) || isnan(Temp)) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Error");
    return;
  }

  temperature[7]     = Temp / 10 + 48; //7th character of the array is equal to the tens of temperature value
  temperature[8]     = Temp % 10 + 48;  //8th character of the array is equal to the ones of temperature value
  temperature[11]    = 223; //adding the degree symbol (°) in ASCII
  humidity[7]        = RH / 10 + 48; //7th character of the array is equal to the tens of humidity value
  humidity[8]        = RH % 10 + 48; //8th character of the array is equal to the tens of humidity value
  lcd.setCursor(0, 0);
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print(humidity);

  //blinking the LEDs every second
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    if (ledState == LOW)
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }

  tmElements_t tm;
  if (RTC.read(tm)) //read time from RTC
  {
    Hour = tm.Hour;
    if (tm.Hour > 12) //conversion to 12-h format from 24-h format
    {
      if (tm.Hour == 13) Hour = 1;
      if (tm.Hour == 14) Hour = 2;
      if (tm.Hour == 15) Hour = 3;
      if (tm.Hour == 16) Hour = 4;
      if (tm.Hour == 17) Hour = 5;
      if (tm.Hour == 18) Hour = 6;
      if (tm.Hour == 19) Hour = 7;
      if (tm.Hour == 20) Hour = 8;
      if (tm.Hour == 21) Hour = 9;
      if (tm.Hour == 22) Hour = 10;
      if (tm.Hour == 23) Hour = 11;
    }
    else
    {
      if (tm.Hour == 0) Hour = 12;
    }
  }
  number = ((Hour * 10000) + (tm.Minute * 100) +  tm.Second); //calculate a 6-digit integer to pass to the seven segment displays as current time
  Display.setNumber(number);
  Serial.println(number);
  Display.refreshDisplay();
}
