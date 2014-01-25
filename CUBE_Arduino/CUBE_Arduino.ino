  // make an empty tab called ignore.h and this will run.
#include "ignore.h" 
//#include "LPD8806.h"

#include <OctoWS2811.h>

//108 or 116 pixels
const int ledsPerStrip = 108;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 strip(ledsPerStrip, displayMemory, drawingMemory, config);

boolean useHeartbeat = true;

// #include "RTClib.h"
//#include <SPI.h>
//#include <Wire.h>

// #define wireAddress 1 
boolean light = false;

// unsigned long currentTime = 0;
// unsigned long loopTime = 0;

//#ifndef _SERIAL
//#define _SERIAL Uart
//HardwareSerial1 Uart = HardwareSerial1();
//#endif

// Access to the pixel strip

#define NUM_POLES 6

#define myADDRESS 11
#define mySETADDRESS 2
#define globalADDR 0

//LPD8806 strip = LPD8806(64);

// RTC_DS1307 RTC;

#define NULL_PATTERN 0
#define OFF_PATTERN 68
#define PAUSE_PATTERN 67
#define ADV_PATTERN 101
#define RESET_FRAME 100

#define TIMING_ADDR 100

#define MAX_FRAME 100000

unsigned int incomingBrightness=0;
unsigned int incomingRate=0;
unsigned int rate = 2;
unsigned int patternByte = NULL_PATTERN;

// unix timestamp that the sketch starts at
unsigned long startedAt = 0;
unsigned long lastTime = -1;

float brightness = 1.0;
uint8_t r1 = 255, g1 = 255, b1 = 255, 
r2 = 0, g2 = 0, b2 = 0, 
r3 = 0, g3 = 0, b3 = 255;

float params[20];
uint32_t color1, color2, color3;

boolean isOff = false;
boolean advance = false;

long frame = 0;
long lastFrame = -1;

typedef uint32_t (*Pattern)(long, int);
Pattern patterns[128];
Pattern pattern;

typedef int (*Mapping)(long, int);
Mapping mapping = &forward;

unsigned long currentTime;
unsigned long lastMillis;
unsigned long internalTimeSmoother;

String inputString = "";
boolean stringComplete = false;

void setup() {  
  
  pinMode(13, OUTPUT); 

  Serial1.begin(9600); 
  Serial.begin(9600);
  
  strip.begin();
  
  setColors();

  hideAll();
  showAll();

  patterns[62] = &flickerStrobeTwo;
  patterns[63] = &flickerStrobeFour;
  patterns[64] = &totesRandom;
  patterns[65] = &rainbowCycle;
  patterns[66] = &rainbow;
  // 67 = pause
  // 68 = off
  patterns[69] = &solidColor;
  patterns[70] = &gradient;
  patterns[71] = &pulseSine;
  patterns[72] = &pulseSaw;
  patterns[73] = &bounce;
  patterns[74] = &colorWipe;
  patterns[75] = &colorAlternator;
  patterns[76] = &stripe;
  patterns[77] = &colorChase;
  patterns[78] = &colorWipeMeter;
  patterns[79] = &colorWipeMeterGradient;
  patterns[80] = &pulseOnce;

//  pattern = &pulseOnce;
 pattern = &pulseOnce;
 //setColors();
 rate = 50 ;
 brightness = 127;

  pattern(-2, 0);

  startedAt = 0;//RTC.now().unixtime();

  inputString.reserve(200);
  
  /*
  while(1)
  {
   strip.setPixelColor(10, 0, 255, 0);
   strip.show();
  }
  */
}

void read() {
  
  while (Serial1.available()) 
  {
    char c = (char)Serial1.read();
    // Serial.println(c, DEC);
    inputString += c;
    if (c == 128) {


      
      if (inputString.charAt(0) == 129) {

        // Heartbeat.
if(useHeartbeat){
        // Big fat hack to turn a String into an int.
        String sub = inputString.substring(1, inputString.length()-1);
        char c[sub.length()];
        for (int i = 0; i < sub.length(); i++) {
          c[i] = sub.charAt(i);
        }
        currentTime = atol(c);

        // Serial.print("Current time: ");
        // Serial.println(currentTime);
}

      } else { 

        unsigned char addr = (unsigned char)inputString.charAt(0);

//         Serial.println("ADDR");
//         Serial.println(addr);


        // Pattern.
        if (addr == myADDRESS || addr == mySETADDRESS || addr == globalADDR) {
          
          rate = (unsigned char)inputString.charAt(1) + 1;
          patternByte = (unsigned char)inputString.charAt(2);
          
//          Serial.println("PTTRN");
//          Serial.println(patternByte);
          
          r1 = (unsigned char)inputString.charAt(3);
          g1 = (unsigned char)inputString.charAt(4);
          b1 = (unsigned char)inputString.charAt(5);
          r2 = (unsigned char)inputString.charAt(6);
          g2 = (unsigned char)inputString.charAt(7);
          b2 = (unsigned char)inputString.charAt(8);
          r3 = (unsigned char)inputString.charAt(9);
          g3 = (unsigned char)inputString.charAt(10);
          b3 = (unsigned char)inputString.charAt(11);

          brightness = ((unsigned char)inputString.charAt(12))/127.0;
Serial.println("BRIGHTNESS");
          Serial.println(brightness);
          setColors();

          if (patternByte == 1) {
            mapping = &forward;
          } 
          else if (patternByte == 2) {
            mapping = &backward;
          } 
          else if (patternByte == 3) {
            mapping = &peak;
          } 
          else if (patternByte == 4) {
            mapping = &valley;
          } 
          else if (patternByte == 5) {
            mapping = &dither;
          } 
          else if (patternByte == OFF_PATTERN) {
            hideAll();
            showAll();
            isOff = true;
          } 
          else if (patternByte != NULL_PATTERN && patterns[patternByte] != NULL) {
            isOff = false;
            pattern = patterns[patternByte];
            pattern(-2, 0); // On select initialization
          }

        }

      }

      inputString = "";

    }

  }



}


void setColors() 
{
  
  color1 = strip.Color(r1, g1, b1);
  color2 = strip.Color(r2, g2, b2);
  color3 = strip.Color(r3, g3, b3);
  
}

void setBrightnRate() {

  rate = incomingRate;
  brightness = incomingBrightness;

}

void loop() {

  read();

  if (isOff){
    hideAll();
    showAll();
    return;
  }
 
  unsigned long currentMillis = millis();

  if (currentTime != lastTime) {
    internalTimeSmoother = 0;
  }

  internalTimeSmoother += currentMillis - lastMillis;

  lastMillis = currentMillis;
  lastTime = currentTime;

  // int t = (currentTime + timesCycled * 256);

  frame = (currentTime + internalTimeSmoother) / rate;


  // if (currentTime >= loopTime + rate) { 

  if (frame != lastFrame)
    pattern(-1, 0); // Per frame initialization

  lastFrame = frame;

  for (int i = 0; i < strip.numPixels(); i++) {

    int j = mapping(frame, i);
    uint32_t color = pattern(frame, j);

  
   uint8_t r = ((color & 0xFF0000) >> 16);
    uint8_t g = ((color & 0x00FF00) >> 8);
    uint8_t b = ((color & 0x0000FF));
    
    if (brightness < 1) {
      r = lerp(0, r, brightness);
      g = lerp(0, g, brightness);
      b = lerp(0, b, brightness);
    }
    
strip.setPixelColor(i, r * 2, g * 2, b * 2);


    //      if (i == 0) {
    //        Serial1.println("color ");
    //        Serial1.println(r);
    //        Serial1.println(g);
    //        Serial1.println(b);
    //        Serial1.println("frame ");
    //        Serial1.println(frame);
    //        
    //        Serial1.println("===================== ");
    //      }

  }

  showAll();  

  if (frame >= MAX_FRAME) { 
    frame = 0;
  } 
    // frame++;

    // loopTime = currentTime;  // Updates loopTime


  // }
//advance = false;



  if (light)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
    
  light = !light;

}


// void loop(){
// }

/* Helper functions */

//Input a rateue 0 to 384 to get a color rateue.
//The colours are a transition r - g -b - back to r

void hideAll() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0); 
  }
}

void showAll(){
  strip.show();    
}


