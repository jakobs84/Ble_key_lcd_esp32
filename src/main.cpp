#include <Arduino.h>
#include <Credentials.h>
#include "version.h"

#include <Keypad.h>

#include <TFT_eSPI.h> 
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"

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

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

/*
#ifdef TOUCH_CS
#define(DISABLE_ALL_LIBRARY_WARNINGS)
#endif
*/


#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms) //use-> espDelay(6000);
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

#include <Credentials.h>

const char* ssid       = WIFI_SSID;               // WiFi SSID     replace with details for your local network
const char* password   = WIFI_PW;           // WiFi Password replace with details for your local network


void setup() {
  Serial.begin(115200);

  Serial.printf("\n\nBLE_KEY_LCD %s compiled %s %s %s\n", VERSION, __DATE__, __TIME__, __FILE__);

/*
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();      //  https://starter-kit.nettigo.pl/2018/02/nodemcu-praca-trybie-ap-czyli-wlasne-wifi/
  delay(500);
  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi..  ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
    delay(1000);
  */
  
    tft.init();
    tft.fontHeight(2);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Hello world", tft.width()/4, tft.height() / 2, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}

}

void loop() {
    char key = keypad.getKey();
  
  if (key){
    Serial.println(key);
    tft.fillScreen(TFT_BLACK);
    tft.drawChar(key, tft.width()/4, tft.height() / 2, 4);
    //tft.drawString(key, tft.width()/4, tft.height() / 2, 4);
  }

 

}

