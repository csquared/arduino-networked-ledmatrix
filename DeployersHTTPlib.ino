#include <SPI.h>
#include <Ethernet.h>

#include "HT1632.h"
#include "HTTP.h"

// LED Matrix Pins
#define DATA 2
#define WR   3
#define CS   4
#define CS2  5
#define CS3  6
#define CS4  7
#define CS5  8
#define CS6  9  

//Led Matrix Controller
HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4, CS5, CS6);
HTTP heroku = HTTP("deployers.herokuapp.com");

//Display magic variables
int x_offset = 2;
int y_offset = 4;
int display_length_millis = 1000;

//Setup
void setup() {
  Serial.begin(9600);
  matrix.begin(HT1632_COMMON_16NMOS);
  matrix.setTextSize(1);    // size 1 == 8 pixels high
  matrix.setTextColor(1);   // 'lit' LEDs
  
  matrix.setCursor(x_offset, y_offset); 
  matrix.print("DHCP...");
  matrix.writeScreen();
  Serial.println("dhcp");
  
  // Connect via DHCP
  byte mac[] = { 0xDE, 0xDE, 0xDE, 0xEF, 0xFE, 0xED };
  if (heroku.begin(mac) == 0) {
     Serial.println("Failed to configure Ethernet using DHCP");
  }
  
  matrix.clearScreen();
  matrix.setCursor(x_offset, y_offset); 
  matrix.print("Connecting...");
  matrix.writeScreen();
  Serial.println("Setup!");
}

String response;
String app_name;

//Variables for loop()
void loop() {
  response = "";

  //connect to HTTP
  if(heroku.get("/", &response) == 0){
    app_name = "";
    
    for(int i = 0; i < response.length(); i++){
      if(response.charAt(i) == ','){
        Serial.print("Print: ");
        Serial.println(app_name);
        matrix.clearScreen();
        matrix.setCursor(x_offset, y_offset); 
        matrix.print(app_name);
        matrix.writeScreen();
        app_name  = "";
        delay(display_length_millis);
      }else{
        app_name.concat(response.charAt(i));  
      }
    }
  }else{
    //wait before next attempt to connect
    delay(display_length_millis);  
  }
}


