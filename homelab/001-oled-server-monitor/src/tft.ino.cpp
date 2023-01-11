#include "TFT_eSPI.h"
#include "FreeSansBold7pt7b.h"
#include "ArduinoJson-v6.20.0.h"
#include <stdio.h>
#include <string.h>
#include <regex.h>

#define START_BYTE 0x01
#define END_BYTE 0x00
#define ARRAY_SIZE 200

TFT_eSPI tft= TFT_eSPI();
TFT_eSprite legend = TFT_eSprite(&tft);
TFT_eSprite plot = TFT_eSprite(&tft);

int oled_width = 320;
int oled_height = 170;
int legend_width = 95;
int margin = 5;
int num_points = ARRAY_SIZE;

int cpu[ARRAY_SIZE];
int mem[ARRAY_SIZE];
int net_in[ARRAY_SIZE];
int net_out[ARRAY_SIZE];
int hdd_read[ARRAY_SIZE];
int hdd_write[ARRAY_SIZE];
int temp[ARRAY_SIZE];

String cpu_desc = "CPU";
String mem_desc = "RAM";
String net_in_desc = "NET in";
String net_out_desc = "NET out";
String hdd_read_desc = "HDD read";
String hdd_write_desc = "HDD write";
String temp_desc = "TEMP";

// get start time
unsigned long last_update = millis() - 20000;

// fill all the arrays with -1 values
void fillArray(int array[], int size) {
  for (int i = 0; i < size; i++) {
    array[i] = 101;
  }
}

char receivedChars[ARRAY_SIZE + 35];
boolean newData = false;
String error = "no error";
String message = "Serial not available";
String result = "Serial not available";
int message_length = 0;
int iteration = 0;

// define colors

word convertRgb( byte R, byte G, byte B) {
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

void plotLegend() {
  legend.setTextDatum(3);
  int rect_width = legend_width - 2 * margin;
  int rect_height = 20;
  int round = 4;
  int rect_x = margin;
  int rect_y = margin;
  rect_y = margin + 5 + 0 * (rect_height + 5);
  legend.fillRoundRect(rect_x,rect_y,rect_width,rect_height,round,darkGreen);
  legend.drawString("CPU", margin + 10, rect_y + 9);
  rect_y = margin + 5 + 1 * (rect_height + 5);
  legend.fillRoundRect(rect_x,rect_y,rect_width,rect_height,round,lightGreen);
  legend.drawString("RAM", margin + 10, rect_y + 9);
  rect_y = margin + 5 + 2 * (rect_height + 5);
  legend.fillRoundRect(rect_x,rect_y,rect_width,rect_height,round,darkOrange);
  legend.drawString("NET in", margin + 10, rect_y + 9);
  rect_y = margin + 5 + 3 * (rect_height + 5);
  legend.fillRoundRect(rect_x,rect_y,rect_width,rect_height,round,lightOrange);
  legend.drawString("NET out", margin + 10, rect_y + 9);
  rect_y = margin + 5 + 4 * (rect_height + 5);
  legend.fillRoundRect(rect_x,rect_y,rect_width,rect_height,round,darkRed);
  legend.drawString("HDD write", margin + 10, rect_y + 9);
  rect_y = margin + 5 + 5 * (rect_height + 5);
  legend.fillRoundRect(rect_x,rect_y,rect_width,rect_height,round,lightRed);
  legend.drawString("HDD read", margin + 10, rect_y + 9);
}

// setup



void setup() {

  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(dark);

  legend.createSprite(legend_width,oled_height); 
  legend.setTextColor(light);
  legend.setFreeFont(&FreeSansBold7pt7b);
  legend.fillSprite(dark);
  plotLegend();
  legend.pushSprite(0,0);

  plot.createSprite(oled_width - legend_width,oled_height);
  plot.setTextColor(light);
  plot.fillSprite(dark);
  plot.setFreeFont(&FreeSansBold7pt7b);
  plot.pushSprite(legend_width,0);

  fillArray(cpu, ARRAY_SIZE);
  fillArray(mem, ARRAY_SIZE);
  fillArray(net_in, ARRAY_SIZE);
  fillArray(net_out, ARRAY_SIZE);
  fillArray(hdd_read, ARRAY_SIZE);
  fillArray(hdd_write, ARRAY_SIZE);
  fillArray(temp, ARRAY_SIZE);

}

void plotAxis() {
  int right_axis = ARRAY_SIZE + margin - 1;
  int bracket = 5;
  plot.drawFastHLine(margin, 10, bracket, gray);
  plot.drawFastHLine(margin, 155, bracket, gray);
  plot.drawFastHLine(right_axis - bracket, 10, bracket, gray);
  plot.drawFastHLine(right_axis - bracket, 155, bracket, gray);
  plot.drawFastVLine(margin, 10, 145, gray);
  plot.drawFastVLine(right_axis, 10, 145, gray);
}

String replace_char(String str, char find, char replace){
    //convert String to char*
    int len = str.length() + 1;
    char array_char[len];
    str.toCharArray(array_char, str.length() + 1);
    char *current_pos = strchr(array_char,find);
    while (current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return String(array_char);
}

void plotLineFill(int* y_data1, int* y_data2, int color1, int color2, String text1, String text2) {
  int left_margin = 6;
  int bottom = 155;
  int x_data[ARRAY_SIZE];

  // set maximum for y_data1 to 100 minimu to 0 and scale y_data1 100 to 145;
  int last_x = left_margin;
  int last_y1 = int(map(max(min(y_data1[0], 100), 0), 0, 100, 0, 145));
  int last_y2 = int(map(max(min(y_data2[0], 100), 0), 0, 100, 0, 145));
  int x = last_x;
  int y1 = last_y1;
  int y2 = last_y2;

  for (int i = 1; i < ARRAY_SIZE - 2; i++) {
    x = i + left_margin;
    y1 = int(map(max(min(y_data1[i], 100),0), 0, 100, 0, 145));
    y2 = int(map(max(min(y_data2[i], 100),0), 0, 100, 0, 145));
  
    plot.fillRect(last_x, bottom - y1, 2, y1, color1);
    plot.fillRect(last_x, bottom - y2, 2, y2, color2);
    plot.drawLine(last_x, bottom - last_y1 - 1, x, bottom - y1 - 1, color1);
    plot.drawLine(last_x, bottom - last_y1 - 2, x, bottom - y1 - 2, color1);
    plot.drawLine(last_x, bottom - last_y1 - 3, x, bottom - y1 - 3, color1);
    plot.drawLine(last_x, bottom - last_y2 - 1, x, bottom - y2 - 1, color2);
    plot.drawLine(last_x, bottom - last_y2 - 2, x, bottom - y2 - 2, color2);
    plot.drawLine(last_x, bottom - last_y2 - 3, x, bottom - y2 - 3, color2);

    last_x = x;
    last_y1 = y1;
    last_y2 = y2;
  }
  // set text datum align to left and baseline
  plot.setTextDatum(3);
  plot.setTextColor(color1);
  plot.drawString(replace_char(text1, '_', ' '), margin + 10, 15);
  // set text datum align to right and baseline
  plot.setTextDatum(5);
  plot.setTextColor(color2);
  plot.drawString(replace_char(text2, '_', ' '), ARRAY_SIZE + margin - 10, 15);
}

// fill array with ints based on ascii values
void fillArrayWithAscii(int* array, String ascii_string) {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (ascii_string.length() > i) { 
      char c = ascii_string.charAt(i);
      int ascii = (int) c - 33;
      array[i] = ascii;
    }    
  }
}


void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static int ndx = 0;

  char rc;
  error = "serial not available";
  // wait for serial data
  while (Serial.available() == 0) {
    error = "serial not available";
    delay(50);
    // if time from last update is greater than 20 seconds print an error message
    int time_since_last_update = int((millis() - last_update) / 1000);
    if (time_since_last_update > 20) {
      plot.fillSprite(dark);
      plot.setTextDatum(3);
      plot.drawString("NO DATA FOR " + String(time_since_last_update) + "s",20,75);
      plot.pushSprite(legend_width,0);
      
    }

  }
  if (Serial.available() > 0) {
    last_update = millis();
    error = "Start";
    recvInProgress = true;
  }
  int chunkId = 0;
  while (recvInProgress) {
    last_update = millis();
    error = "recvInProgress";
    if (Serial.available() > 0) {
      error = "receiving";
      rc = Serial.read();
      if (ndx == 0) {
        recvInProgress = true;
      }
      if (rc != END_BYTE) {
        error = "not end marker";
        receivedChars[ndx] = rc;
        ndx++;
        message_length = ndx;
      }
      else {
        error = "terminated";
        String msg = String(receivedChars);
        int splitIndex = msg.indexOf(' ');
        String desc = msg.substring(0, splitIndex);
        int descSplitIndex = desc.indexOf(':');
        String word = desc.substring(0, descSplitIndex);
        desc = desc.substring(descSplitIndex + 1, desc.length());
        String asciiCharacters = msg.substring(splitIndex + 1, splitIndex + 1 + ARRAY_SIZE);
        message = word;
        //get size of ascii characters
        int size = asciiCharacters.length();
        result = String(size) + " " + asciiCharacters.substring(size - 10, size);
        result = word + " " + String(cpu[0]) + " " + String(cpu[1]) + " " + String(cpu[2]) + " " + String(cpu[3]);
        if (word == "cpu") {
          cpu_desc = desc;
          fillArrayWithAscii(cpu, asciiCharacters);
        }
        if (word == "mem") {
          mem_desc = desc;
          fillArrayWithAscii(mem, asciiCharacters);
        }
        if (word == "network_recv") {
          net_in_desc = desc;
          fillArrayWithAscii(net_in, asciiCharacters);
        }
        if (word == "network_sent") {
          net_out_desc = desc;
          fillArrayWithAscii(net_out, asciiCharacters);
        }
        if (word == "disk_read") {
          hdd_read_desc = desc;
          fillArrayWithAscii(hdd_read, asciiCharacters);
        }
        if (word == "disk_write") {
          hdd_write_desc = desc;
          fillArrayWithAscii(hdd_write, asciiCharacters);
        }
        if (word == "temp") {
          temp_desc = desc;
          fillArrayWithAscii(temp, asciiCharacters);
        }

        recvInProgress = false;
        ndx = 0;
      }
    }
    delay(1);
  }
  chunkId++;
  error = "chunks received: " + String(chunkId);
  

}

int presentation = 0;

void loop() {

  recvWithStartEndMarkers();

      
  plot.fillSprite(dark);
  plotAxis();
  // if cpu is not empty, plot it
  if (cpu[0] < 101 && mem[0] < 101 && presentation % 4 == 0) {
    message = "printing cpu";
    plotLineFill(mem, cpu, lightGreen, darkGreen, mem_desc, cpu_desc);
  }
  if (net_in[0] < 101 && net_out[0] < 101 && presentation % 4 == 1) {
    message = "printing network";
    plotLineFill(net_in, net_out, darkOrange, lightOrange, net_in_desc, net_out_desc);
  }
  if (hdd_read[0] < 101 && hdd_write[0] < 101 && presentation % 4 == 2) {
    message = "printing disk";
    plotLineFill(hdd_read, hdd_write, darkRed, lightRed, hdd_read_desc, hdd_write_desc);
  }
  if (temp[0] < 101 && presentation % 4 == 3) {
    message = "printing temp";
    plotLineFill(temp, temp, darkViolet, darkViolet, temp_desc, "");
  }
  
  
  plot.setTextDatum(3);
  // plot.drawString("Result: " + result,20,50);
  // plot.drawString("Ms: " + message + ".",20,75);
  // plot.drawString("Length: " + String(message_length),20,100);
  // plot.drawString("Iteration: " + String(iteration),20,125);
  // plot.drawString("Error: " + error,20,150);
  plot.pushSprite(legend_width,0);

  iteration++;
  if (iteration % 10 == 0) {
    presentation++;
    if (presentation > 3) {
      presentation = 0;
    }
  }
  // reset iteration if closing to int overflow
  if (iteration > 10000000) {
    iteration = 0;
  }
  delay(10); 

}