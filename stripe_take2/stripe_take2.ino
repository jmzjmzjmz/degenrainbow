 // make an empty tab called ignore.h and this will run.
#include "ignore.h" 

// #include "LPD8806.h"
// LPD8806 strip = LPD8806(60);
#include <SPI.h>
#include <FastSPI_LED.h>


//#include <Adafruit_NeoPixel.h>
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(116, 6, NEO_GRB + NEO_KHZ400);



boolean light = false;

// unsigned long currentTime = 0;
// unsigned long loopTime = 0;

//#ifndef _SERIAL
//#define _SERIAL Uart
//HardwareSerial1 Uart = HardwareSerial1();
//#endif

struct CRGB { 

  unsigned char g; 
  unsigned char r; 
  unsigned char b; 

};

struct CRGB *leds;
struct CRGB *temp_leds;
int _i = 0, j = 0, k = 0;
#define PIN 4

// Access to the pixel strip

#define NUM_POLES 8

#define myADDRESS 4
#define mySETADDRESS 10
#define globalADDR 0

/* These don't seem to work as #define since they're used in other tabs ... */
//STRIPE 4 top 180


unsigned int LEAD_ROW = 264;
unsigned int MAIN_ROW = 180; //264;//32; 
unsigned int BOT_ROW = 154; //238;// 18; 
unsigned int NUM_PIXELS = BOT_ROW + MAIN_ROW;
//switch out MAIN_ROW with LEAD_ROW to effect how many pixels the pattern takes into account
unsigned int PATTERN_ROW = MAIN_ROW;

int bottom_map[264+238];

struct CRGB color;
struct CRGB NULL_COLOR;

#define NULL_PATTERN 0
#define OFF_PATTERN 68
#define PAUSE_PATTERN 67
#define ADV_PATTERN 101
#define RESET_FRAME 100

#define TIMING_ADDR 100

#define MAX_FRAME 100000

unsigned int incomingBrightness=0;
unsigned int incomingRate=0;
unsigned int rate = 50;
unsigned int patternByte = NULL_PATTERN;

// unix timestamp that the sketch starts at
unsigned long startedAt = 0;
unsigned long lastTime = -1;

float brightness = 1.0;
int r1 = 127,  g1 = 0,  b1 = 0, 
    r2 = 0,   g2 = 0,   b2 = 127, 
    heartOffset = 0,   xFader = 0,   scaling = 0;

float params[20];
struct CRGB color1, color2, color3;

boolean isOff = false;
boolean advance = false;

long frame = 0;
long lastFrame = -1;

// typedef uint32_t (*Pattern)(long, int);
typedef CRGB (*Pattern)(long, int);
Pattern patterns[128];
Pattern pattern;

typedef int (*Mapping)(long, int);
Mapping mapping = &peak;

unsigned long currentTime;
unsigned long lastMillis;
unsigned long internalTimeSmoother;

String inputString = "";
boolean stringComplete = false;




void setup() {  
  
  pinMode(13, OUTPUT); 

  Serial1.begin(9600); 
  Serial.begin(9600);
  
  FastSPI_LED.setLeds(NUM_PIXELS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_LPD8806);
  FastSPI_LED.setPin(PIN);
  FastSPI_LED.setDataRate(3);
  FastSPI_LED.init();
  FastSPI_LED.start();
  leds = (struct CRGB*)FastSPI_LED.getRGBData(); 

  //SLOW DOWN SPI DATA RATE FOR LONG LENGTHS OF CABLING

  SPI0_CTAR0 &= 0x7FFFFFFF; //mask register

  //SPI0_CTAR0 |= 0x00000000; //<====== HERE

//Create mapping scale array
for(int i = MAIN_ROW; i < NUM_PIXELS; i++){
bottom_map[i] = (int)fscale(i,MAIN_ROW,NUM_PIXELS - 1,MAIN_ROW -1,0,-.05);
}

  
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

  pattern = &rainbowCycle;
  pattern(-2, 0);

  startedAt = 0;

  inputString.reserve(200);

}

void read() {
  
  while (Serial.available()) {


    char c = (char)Serial.read();
    // Serial.println(c, DEC);
    inputString += c;
    if (c == 128) {


      
      if (inputString.charAt(0) == 129) {

        // Heartbeat.

        // Big fat hack to turn a String into an int.
        String sub = inputString.substring(1, inputString.length()-1);
        char c[sub.length()];
        for (int i = 0; i < sub.length(); i++) {
          c[i] = sub.charAt(i);
        }
        currentTime = atol(c);

        // Serial.print("Current time: ");
        // Serial.println(currentTime);


      } else { 

        unsigned char addr = (unsigned char)inputString.charAt(0);

        // Serial.println("PATTERN");
        // Serial.println(addr);


        // Pattern.
        if (addr == myADDRESS || addr == mySETADDRESS || addr == globalADDR) {
        
          rate = (unsigned char)inputString.charAt(1);// + 1;
          patternByte = (unsigned char)inputString.charAt(2);

          r1 = (unsigned char)inputString.charAt(3);
          g1 = (unsigned char)inputString.charAt(5);
          b1 = (unsigned char)inputString.charAt(4);
          r2 = (unsigned char)inputString.charAt(6);
          g2 = (unsigned char)inputString.charAt(8);
          b2 = (unsigned char)inputString.charAt(7);
          heartOffset = (unsigned char)inputString.charAt(9);
          xFader = (unsigned char)inputString.charAt(11);
          scaling = (unsigned char)inputString.charAt(10);

          brightness = ((unsigned char)inputString.charAt(12))/127.0;

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

void setColors() {

  color1.r = r1;
  color1.g = g1;
  color1.b = b1;
  
  color2.r = r2;
  color2.g = g2;
  color2.b = b2;
  
  color3.r = r3;
  color3.g = g3;
  color3.b = b3;

}

uint32_t Color(byte r, byte g, byte b) {
  return 0x808080 | ((uint32_t)g << 16) | ((uint32_t)r << 8) | (uint32_t)b;
}

void setBrightnRate() {

  rate = incomingRate;
  brightness = incomingBrightness;

}

boolean freezeBool = false;

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


if(rate != 127){ //FREEEEEEEEEZE (why does processing's 0 also fuck with it?)

  frame = (currentTime + internalTimeSmoother) / rate +1;//+1 to avoid dividing by 0
freezeBool = false;

}



 // if (currentTime >= loopTime + rate) { 

  if (frame != lastFrame)
    pattern(-1, 0); // Per frame initialization

  lastFrame = frame;



//determines what pattern to run on the top strip

  for (_i = 0; _i < MAIN_ROW; _i++) {

  // if(PATTERN_ROW == MAIN_ROW){
    int j = mapping(frame, _i);
//    }
//   // else it should mimic the top most stripe
// else{
//     // int j = mapping(frame, map(_i,0,MAIN_ROW,0,LEAD_ROW));
//     int j = mapping(frame, (int)fscale(_i,0, LEAD_ROW - 1,0,MAIN_ROW - 1, -.05));
// }

color = pattern(frame, j);

  if (brightness < 1) {
      color.r = lerp(0, color.r, brightness);
      color.g = lerp(0, color.g, brightness);
      color.b = lerp(0, color.b, brightness);
    }

    leds[_i].r = color.r;
    leds[_i].g = color.g;
    leds[_i].b = color.b;

  }

//SHORTER,BOTTOM ROW MIMICS TOP 'MAIN' ROW
    // i wanna map i from 180-333 to 179-0
    for (_i = MAIN_ROW; _i < NUM_PIXELS; _i++) {
    leds[_i].r = leds[bottom_map[_i]].r;
    leds[_i].g = leds[bottom_map[_i]].g;
    leds[_i].b = leds[bottom_map[_i]].b;
  }


if(freezeBool == false){
  showAll();  
}

if(patternByte ==64 && rate == 127){
  showAll();
freezeBool = true;
}


  if (frame >= MAX_FRAME) { 
    frame = 0;
  } 
    



  if (light)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
    
  light = !light;

}



/* Helper functions */

//Input a rateue 0 to 384 to get a color rateue.
//The colours are a transition r - g -b - back to r

void hideAll() {
 memset(leds, 0, NUM_PIXELS * 3);
}

void showAll(){
  FastSPI_LED.show();
}

