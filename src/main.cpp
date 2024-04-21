#include <Arduino.h>
//#include <Credentials.h>
#include "Credentials.h"
#include "version.h"

#include <Keypad.h>

//#define DISABLE_ALL_LIBRARY_WARNINGS_TFT



#include <TFT_eSPI.h> 
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"
//#include <esp32-hal-sleep.h>
//#include <esp_sleep.h>
#include <esp_deep_sleep.h>

#include <BleKeyboard.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {21,27,26,22}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {33,32,25}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//Add enter -- mising in BleKeyboard.h
const uint8_t KEY_ENTER = 0xE0;
const uint8_t KEY_SPACE = 0x20;

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

char buff[512];
int vref = 1100;
int btnCick = false;
bool preisConnected = 0;
bool isConnected =0;

bool on_tab, prin;

unsigned long startTimeK;
const long interval = 8000;
unsigned long previousMillis = 0;
int tabb_s = 180000;   //180000 ms [600000 = 1 m]
//int tabb_s = 18000;
int tabb_ss = 2000;


TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

bool lcdEnabled = false; // Flaga oznaczająca, czy wyświetlacz jest włączony
unsigned long lastKeyPressTime = 0; // Czas ostatniego naciśnięcia klawisza
short int lcdSllep = 5000;
int lcdSllepdeep = 300000; 
/*
void wifi_scan()
{
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    tft.drawString("Scan Network", tft.width() / 2, tft.height() / 2);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int16_t n = WiFi.scanNetworks();
    tft.fillScreen(TFT_BLACK);
    if (n == 0) {
        tft.drawString("no networks found", tft.width() / 2, tft.height() / 2);
    } else {
        tft.setTextDatum(TL_DATUM);
        tft.setCursor(0, 0);
        Serial.printf("Found %d net\n", n);
        for (int i = 0; i < n; ++i) {
            sprintf(buff,
                    "[%d]:%s(%d)",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i));
            tft.println(buff);
        }
    }
    WiFi.mode(WIFI_OFF);
}

void showVoltage()
{
  //https://github.com/JakubAndrysek/TTGO_T_Display/blob/master/TTGO_example/src/main.cpp

    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 1000) {
        timeStamp = millis();
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        String voltage = "Voltage :" + String(battery_voltage) + "V";
        Serial.println(voltage);
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(voltage,  tft.width() / 2, tft.height() / 2 );
    }
}

*/

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms) //use-> espDelay(6000);
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}



#include <Credentials.h>

//const char* ssid       = WIFI_SSID;               // WiFi SSID     replace with details for your local network
//const char* password   = WIFI_PW;           // WiFi Password replace with details for your local network

//BleKeyboard bleKeyboard;
//BleKeyboard bleKeyboard("RAPOO BT Receiver", "Rapo", 75);
BleKeyboard bleKeyboard(Name, Company, 75); // from version.h


void setup() {
  Serial.begin(115200);

  Serial.printf("\n\nBLE_KEY_LCD %s compiled %s %s %s\n", VERSION, __DATE__, __TIME__, __FILE__);
 
    tft.init();
    tft.fontHeight(2);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    //tft.drawString("Hello world", tft.width()/4, tft.height() / 2, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}


    bleKeyboard.begin();

}




void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
     //doBtCheck();
    isConnected = bleKeyboard.isConnected();
    if (preisConnected != isConnected)
    {
      if(isConnected){
      tft.drawChar('*', 225, 1, 4);
      } else {
      tft.drawChar('^', 225, 2, 4);
      }
      preisConnected = isConnected;
    }

  }
  

  
char key = keypad.getKey();
  if (key){
    tft.fillScreen(TFT_BLACK); // czyście ekran 
    if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
         pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
         digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
    lcdEnabled = true; // Włącz wyświetlacz flaga
    tft.writecommand(TFT_DISPON); // Włącz podświetlenie wyświetlacza]
    tft.writecommand(TFT_SLPOUT);
    lastKeyPressTime = millis(); // Zapisz czas ostatniego naciśnięcia klawisza    
  }

  if (lcdEnabled && millis() - lastKeyPressTime > lcdSllep) {

   digitalWrite(TFT_BL, 0);
   lcdEnabled = false;
    tft.writecommand(TFT_DISPOFF); // Wyłącz wyświetlacz
    tft.writecommand(TFT_SLPIN); // Ustaw ESP32 w tryb uśpienia
  }
  if (millis() - lastKeyPressTime > lcdSllepdeep) {
     Serial.println("Go Deep");
     delay(300);
    esp_deep_sleep_start();
  }

if(isConnected){
    switch (key)
    {
    case '1':
        tft.drawString("RL TX", 5, 5, 4);
        delay(30);
        bleKeyboard.print(LOG1);
        delay(30);
        bleKeyboard.write(KEY_TAB);
        delay(30);
        bleKeyboard.print(PAS1);
        delay(30);
        bleKeyboard.print(PAS11);
        delay(30);
        bleKeyboard.write(KEY_ENTER);
    break;
    case '2':
        tft.drawString("TL TX", 5, 5, 4);
        delay(30);
        bleKeyboard.print(LOG2);
        delay(30);
        bleKeyboard.print(LOG22);
        delay(30);
        bleKeyboard.print(LOG222);
        delay(30);        
        bleKeyboard.write(KEY_TAB);
        delay(30);
        bleKeyboard.print(PAS1);
        delay(30);
        bleKeyboard.print(PAS11);
        delay(30);        
        bleKeyboard.write(KEY_RETURN);
    break;
    case '3':
        tft.drawString("Mini Link", 5, 5, 4);
        bleKeyboard.print("mini");
        bleKeyboard.write(KEY_DOWN_ARROW);
        bleKeyboard.write(KEY_ENTER);
        for (int i=0; i<7; i++)
        {
        delay(20);
        bleKeyboard.write(KEY_TAB);
        }
        bleKeyboard.write(KEY_ENTER);
        bleKeyboard.releaseAll();
    break;
    case '4':
        tft.drawString(LOG3 LOG33 LOG333 LOG3333, 5, 5, 4);
        delay(20);
        bleKeyboard.print(LOG3);
        delay(20);
        bleKeyboard.print(LOG33);
        delay(20);
        bleKeyboard.print(LOG333);
        delay(20);
        bleKeyboard.print(LOG3333);
        delay(20);

    break;
    case '5':
    tft.drawString("Ericsson1234", 5, 5, 4);
        bleKeyboard.print(PAS1);
        delay(30);
        bleKeyboard.print(PAS11);
        delay(30);
    break;
    case '6':

    break;
    case '7':
    tft.drawString("Firefox", 5, 5, 4);
    bleKeyboard.press(KEY_TAB);
    bleKeyboard.write(KEY_ENTER);
      for (int i=0; i<4; i++)
      {
      delay(5);
      bleKeyboard.write(KEY_TAB);
      }
    bleKeyboard.write(KEY_ENTER);
    delay(5);
    break;
    case '8':
        tft.drawString("RL TX", 5, 5, 4);
        delay(30);
        bleKeyboard.print(LOG1);
        delay(30);
        bleKeyboard.write(KEY_TAB);
        delay(30);
        bleKeyboard.print(PAS1);
        delay(30);
        bleKeyboard.print(PAS11);
        delay(30);
        bleKeyboard.write(KEY_ENTER);
        delay(40);
        bleKeyboard.press(KEY_LEFT_CTRL);
        bleKeyboard.press(KEY_TAB);
        bleKeyboard.releaseAll();
        delay(100);
        bleKeyboard.press(KEY_TAB);
        bleKeyboard.releaseAll();        
    break;
    case '9':
        tft.drawString("Next", 5, 5, 4);
        delay(40);
        bleKeyboard.press(KEY_LEFT_CTRL);
        bleKeyboard.press(KEY_TAB);
        bleKeyboard.releaseAll();
    break;
    case '0':
        tft.drawChar('!', 225, 1, 4);
    break;
    case '*':
      on_tab = !on_tab;
      if (on_tab){
      startTimeK = millis();
      tft.drawNumber(on_tab, 1, 1, 4);
      } else {
      tft.drawChar('$', 1, 1, 4);
      }
    break;

    default:
    tft.drawChar(key, 3, 3, 4);
    break;
    }
    
}


if (on_tab)
    if ( (millis()-startTimeK)>tabb_s ) {       
          startTimeK = millis();
          tft.drawString("Boom", 5, 5, 4);
          bleKeyboard.press(KEY_LEFT_ALT);
          bleKeyboard.press(KEY_TAB);
          delay(100);
          bleKeyboard.releaseAll();
          delay(100);
          bleKeyboard.press(KEY_LEFT_ALT);
          bleKeyboard.press(KEY_TAB);
          delay(100);
          bleKeyboard.releaseAll();

    } else {
            if ( (millis()-startTimeK)>tabb_ss ) {       
            tft.fillScreen(TFT_BLACK);            
            }
    }
}
