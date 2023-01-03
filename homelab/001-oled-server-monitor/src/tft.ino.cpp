#include "TFT_eSPI.h"
#include "FreeSansBold7pt7b.h"

TFT_eSPI tft= TFT_eSPI();
TFT_eSprite legend = TFT_eSprite(&tft);
TFT_eSprite plot = TFT_eSprite(&tft);

word convertRgb( byte R, byte G, byte B)
{
  return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );
}

#define dark convertRgb(0x11, 0x13, 0x11)
#define gray convertRgb(0x90, 0x90, 0x90)
#define light convertRgb(0xE8, 0xE0, 0xDB)
#define darkBlue convertRgb(0x22, 0x6D, 0x66)
#define lightBlue convertRgb(0x38, 0xAC, 0xA9)
#define darkGreen convertRgb(0x65, 0x86, 0x83)
#define lightGreen convertRgb(0x9A, 0xC3, 0xAE)
#define darkOrange convertRgb(0xE6, 0x9C, 0x50)
#define lightOrange convertRgb(0xF4, 0xB8, 0x44)
#define darkRed convertRgb(0xCB, 0x64, 0x62)
#define lightRed convertRgb(0xE1, 0x86, 0x80)
#define darkViolet convertRgb(0x59, 0x50, 0x7B)
#define lightViolet convertRgb(0x98, 0x83, 0xA3)




void setup() {

  Serial.begin(9600);

  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(dark);

  legend.createSprite(120,170); 
  legend.setTextColor(light);
  legend.setFreeFont(&FreeSansBold7pt7b);
  legend.setTextDatum(3);
  legend.fillSprite(dark);

  legend.fillRoundRect(5,10,105,20,4,darkGreen);
  legend.drawString("CPU " + String(4) + " cores", 15, 19);
  legend.fillRoundRect(5,35,105,20,4,lightGreen);
  legend.drawString("RAM " + String(4) + " GB", 15, 44);
  legend.fillRoundRect(5,60,105,20,4,darkOrange);
  legend.drawString("NET in", 15, 69);
  legend.fillRoundRect(5,85,105,20,4,lightOrange);
  legend.drawString("NET out", 15, 94);
  legend.fillRoundRect(5,110,105,20,4,darkRed);
  legend.drawString("HDD write", 15, 119);
  legend.fillRoundRect(5,135,105,20,4,lightRed);
  legend.drawString("HDD read", 15, 144);
  legend.pushSprite(0,0);

  plot.createSprite(200,170);
  plot.setTextColor(light);
  plot.fillSprite(dark);
  plot.setFreeFont(&FreeSansBold7pt7b);
  plot.setTextDatum(2);

  plot.drawFastHLine(5, 10, 5, gray);
  plot.drawFastHLine(5, 155, 5, gray);
  plot.drawFastHLine(190, 10, 5, gray);
  plot.drawFastHLine(190, 155, 5, gray);
  // then plot the y axis
  plot.drawFastVLine(5, 10, 145, gray);
  plot.drawFastVLine(195, 10, 145, gray);
  // the plot the array using loop
  
  int left_margin = 6;
  int bottom = 155;
  int num_points = 190;
  int x_data[num_points];
  int y_data1[num_points];
  int y_data2[num_points];
  for (int i = 0; i < num_points; i++) {
    x_data[i] = i + left_margin;
    if (i == 0) {
      y_data1[i] = 100;
      y_data2[i] = 60;
    } else {
      y_data1[i] = y_data1[i - 1] + random(-3, 3);
      y_data2[i] = y_data2[i - 1] + random(-2, 3);
      plot.fillRect(x_data[i - 1], bottom - y_data1[i], 1, y_data1[i], darkGreen);
      plot.fillRect(x_data[i - 1], bottom - y_data2[i], 1, y_data2[i], lightGreen);
      plot.drawLine(x_data[i - 1], bottom - y_data1[i], x_data[i], bottom - y_data1[i], darkGreen);
      plot.drawLine(x_data[i - 1], bottom - y_data1[i] + 1, x_data[i], bottom - y_data1[i] + 1, darkGreen);
      plot.drawLine(x_data[i - 1], bottom - y_data1[i] + 2, x_data[i], bottom - y_data1[i] + 2, darkGreen);
      plot.drawLine(x_data[i - 1], bottom - y_data2[i], x_data[i], bottom - y_data2[i], lightGreen);
      plot.drawLine(x_data[i - 1], bottom - y_data2[i] + 1, x_data[i], bottom - y_data2[i] + 1, lightGreen);
      plot.drawLine(x_data[i - 1], bottom - y_data2[i] + 2, x_data[i], bottom - y_data2[i] + 2, lightGreen);
    }
  }

  plot.pushSprite(120,0);
  


}

void loop() {

  if (Serial.available() > 0) {

    String message = Serial.readStringUntil('\n');

    plot.fillSprite(dark);
    plot.fillRoundRect(10,10,100,100,4,darkBlue);
    plot.drawString("->: " + message,50,30);
    plot.pushSprite(0,0);

  }

  delay(1000); 

}