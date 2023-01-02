#include "TFT_eSPI.h"
#include "fonts.h"

TFT_eSPI tft= TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

#define dark 0x313331
#define light 0xE8E0DB
#define green  
#define gray 0x6B6D

#define blue 0x4444FF
#define orange 0xC260
#define purple 0x604D
#define green 0x1AE9



void setup() {

  Serial.begin(9600);

  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  //tft.pushImage(106,0,214,170,install);

  sprite.createSprite(146,170); 
  sprite.setTextColor(TFT_WHITE);
  sprite.setTextDatum(4);


 sprite.fillSprite(TFT_BLACK);
 sprite.fillRoundRect(10,10,100,100,4,blue);
 sprite.pushSprite(0,0);

}

//progress bar variables
int progress=0;
int w=120;
int h=18;
int x=12;
int y=90;
int blocks=0;

void loop() {

 progress++;
 // read String message from serial port and show it on the screen
  if (Serial.available() > 0) {
    // String message = Serial.readStringUntil('\n');

    sprite.fillSprite(TFT_BLACK);
    sprite.fillRoundRect(10,10,100,100,4,blue);
    sprite.pushSprite(0,0);
    // sprite.drawString("->: " + message,50,30);
    // sprite.pushSprite(0,0);
  }
 // sprite.drawString("HELLO" + String(progress),50,30);
 // delay for 5 seconds
 delay(2000);
 
}