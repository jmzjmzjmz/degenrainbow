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


// Access to the pixel strip
#define PIN 4

#define SAMPLE_STRIP 0

#define myADDRESS 4
#define globalADDRESS 0

/* These don't seem to work as #define since they're used in other tabs ... */
//STRIPE 4 top 180

#if SAMPLE_STRIP == 1

  unsigned int SMALLEST_STRIP_LENGTH = 22;
  unsigned int OUTER_STRIP_LENGTH = 24;
  unsigned int INNER_STRIP_LENGTH = 22;

#else

  unsigned int SMALLEST_STRIP_LENGTH = 154;

  #if myADDRESS == 1
  unsigned int OUTER_STRIP_LENGTH = 180;
  unsigned int INNER_STRIP_LENGTH = 154;
  #elif myADDRESS == 2
  unsigned int OUTER_STRIP_LENGTH = 208;
  unsigned int INNER_STRIP_LENGTH = 182;
  #elif myADDRESS == 3
  unsigned int OUTER_STRIP_LENGTH = 236;
  unsigned int INNER_STRIP_LENGTH = 210;
  #elif myADDRESS == 4
  unsigned int OUTER_STRIP_LENGTH = 264;
  unsigned int INNER_STRIP_LENGTH = 238;
  #endif

#endif

unsigned int NUM_PIXELS = INNER_STRIP_LENGTH + OUTER_STRIP_LENGTH;

unsigned int VIRTUAL_LENGTH = SMALLEST_STRIP_LENGTH;

// unsigned int LEAD_ROW = 264;


// unsigned int OUTER_STRIP_LENGTH = 180; //264;//32; 
// unsigned int BOT_ROW = 154; //238;// 18; 
// unsigned int NUM_PIXELS = BOT_ROW + OUTER_STRIP_LENGTH;
//switch out OUTER_STRIP_LENGTH with LEAD_ROW to effect how many pixels the pattern takes into account

int bottom_map[264+238];
int scaled_bottom_map[264+238];
int scaled_top_map[264+238];

struct CRGB color;
struct CRGB prev_color;
struct CRGB NULL_COLOR;

#define NULL_PATTERN 0
#define OFF_PATTERN 68
#define PAUSE_PATTERN 67
#define ADV_PATTERN 101
#define RESET_FRAME 100

#define TIMING_ADDR 100

#define MAX_FRAME 100000


unsigned int rate = 70;
unsigned int patternByte = NULL_PATTERN;
unsigned int mappingByte = NULL_PATTERN;
int r1 = 127,  
    g1 = 0,  
    b1 = 0, 
    r2 = 0,    
    g2 = 0,  
    b2 = 127;
unsigned int frameOffset = 0;
unsigned int crossfadeDuration = 0; // millis
unsigned int prev_crossfadeDuration = 0;
float xfadePosition = 0;
bool fixedLength = false;
bool prev_fixedLength = false;

bool isFading = false;
bool usePrevColors = false;

float brightness = 1.0;


unsigned int prev_frameOffset, 
             prev_rate;

//RainbowMapping prev_rainbowMapping;


int prev_r1,  
    prev_g1, 
    prev_b1,
    prev_r2,  
    prev_g2,
    prev_b2;

float prev_brightness;

// unix timestamp that the sketch starts at
unsigned long startedAt = 0;
unsigned long lastTime = -1;

float params[20];
struct CRGB color1, color2;

boolean isOff = false;
boolean advance = false;

long frame = 0;
long prev_frame;
long lastFrame = -1;

// typedef uint32_t (*Pattern)(long, int);
typedef CRGB (*Pattern)(long, int);
Pattern patterns[128];
Pattern pattern;
Pattern prev_pattern;


typedef int (*Mapping)(long, int);
Mapping mappings[128];
Mapping mapping = &peak;
Mapping prev_mapping;
// linear
// bottomToTop
// topToBottom
// 

unsigned long currentTime;
unsigned long lastCommandTime = 0;
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

  

  //Create look up table for normal bottom stripe
  for(int i = OUTER_STRIP_LENGTH; i < NUM_PIXELS; i++){
    bottom_map[i] = (int)fscale(i,OUTER_STRIP_LENGTH,NUM_PIXELS - 1,OUTER_STRIP_LENGTH -1,0,-.05);
  }



  // //Create look up table for scaled top
  // for(int i = 0; i < OUTER_STRIP_LENGTH; i++){
  //   scaled_top_map[i] = (int)fscale(i,0,OUTER_STRIP_LENGTH - 1,OUTER_STRIP_LENGTH -1, 0, -.05);
  // }

  // //Create look up table for scaled bottom
  // for(int i = OUTER_STRIP_LENGTH; i < NUM_PIXELS; i++){
  //   scaled_bottom_map[i] = (int)fscale(i,OUTER_STRIP_LENGTH,NUM_PIXELS - 1,OUTER_STRIP_LENGTH -1,0,-.05);
  // }



  
  setColors(!usePrevColors);

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
  patterns[73] = &betterRainbowCycle;
  patterns[74] = &colorWipe;
  patterns[75] = &colorAlternator;
  patterns[76] = &stripe;
  patterns[77] = &colorChase;
  patterns[78] = &colorWipeMeter;
  patterns[79] = &colorWipeMeterGradient;
  patterns[80] = &pulseOnce;

  mappings[1] = &forward;
  mappings[2] = &backward;
  mappings[3] = &peak;
  mappings[4] = &valley;
  mappings[5] = &dither;

  pattern = &betterRainbowCycle;
  pattern(-2, 0);

  startedAt = 0;

  inputString.reserve(200);

}

void read() {
  
  while (Serial1.available()) {


    char c = (char)Serial1.read();
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
        if ((addr == myADDRESS || addr == globalADDRESS) && !isFading) {

          // Pattern temp1 = prev_pattern;
          // Mapping temp2 = prev_mapping;

          prev_frameOffset = frameOffset;
          prev_rate = rate;
          prev_fixedLength = fixedLength;
          prev_pattern = pattern;
          prev_mapping = mapping;
          prev_brightness = brightness;
          prev_crossfadeDuration = crossfadeDuration;

          prev_r1 = r1;
          prev_g1 = g1;
          prev_b1 = b1;
          prev_r2 = r2;
          prev_g2 = g2;
          prev_b2 = b2;

          rate = (unsigned char)inputString.charAt(1) + 1;
          patternByte = (unsigned char)inputString.charAt(2);
          mappingByte = (unsigned char)inputString.charAt(3);
          r1 = (unsigned char)inputString.charAt(4);
          g1 = (unsigned char)inputString.charAt(6);
          b1 = (unsigned char)inputString.charAt(5);
          r2 = (unsigned char)inputString.charAt(7);
          g2 = (unsigned char)inputString.charAt(9);
          b2 = (unsigned char)inputString.charAt(8);
          frameOffset = (unsigned char)inputString.charAt(10);
          crossfadeDuration = 50.0 * (unsigned char)inputString.charAt(11);
          fixedLength = (unsigned char)inputString.charAt(12) > 0;
          brightness = ((unsigned char)inputString.charAt(13))/127.0;


          if (fixedLength) {
            VIRTUAL_LENGTH = SMALLEST_STRIP_LENGTH;
          } else { 
            VIRTUAL_LENGTH = OUTER_STRIP_LENGTH;
          }

          // if(crossfadeDuration == 0 ){
            setColors(!usePrevColors);
          // }

          if (mappings[mappingByte] != NULL) {
            mapping = mappings[mappingByte];
          }

          if (patternByte == OFF_PATTERN) {
            hideAll();
            showAll();
            isOff = true;
          } 
          else if (patternByte != NULL_PATTERN && patterns[patternByte] != NULL) {
            isOff = false;
            pattern = patterns[patternByte];
            pattern(-2, 0); // On select initialization
          }
//crossfade duration is > 0
          if(prev_pattern != pattern || prev_mapping != mapping) {
            lastCommandTime = currentTime; 
            // Serial.println("new PATTERN/MAPPING");
          }
        }

        // Serial.println("vari'z:");
        // Serial.println(frameOffset);



      


      }

      inputString = "";

    }

     

  }







}

void setColors(bool prev) {

if(prev == true){
  color1.r = r1;
  color1.g = g1;
  color1.b = b1;
  
  color2.r = r2;
  color2.g = g2;
  color2.b = b2;
}
else{
  color1.r = prev_r1;
  color1.g = prev_g1;
  color1.b = prev_b1;
  
  color2.r = prev_r2;
  color2.g = prev_g2;
  color2.b = prev_b2;
}
  // color3.r = r3;
  // color3.g = g3;
  // color3.b = b3;

}

uint32_t Color(byte r, byte g, byte b) {
  return 0x808080 | ((uint32_t)g << 16) | ((uint32_t)r << 8) | (uint32_t)b;
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

  frame = (currentTime + internalTimeSmoother) / rate;//+1 to avoid dividing by 0


  prev_frame = frame + prev_frameOffset;
  
  //frame offset
  frame += frameOffset;

  // if (currentTime >= loopTime + rate) { 

  if (frame != lastFrame) {
    pattern(-1, 0); // Per frame initialization
    if (prev_pattern != NULL) {
      prev_pattern(-1, 0);
    }
  }
  
  lastFrame = frame;

  //determines what pattern to run on the top strip

  for (_i = 0; _i < NUM_PIXELS; _i++) {

    setColors(!usePrevColors);
    
    int k = i2k(_i, fixedLength);
    int j = mapping(frame, k);
    color = pattern(frame, j);

    if (brightness < 1) {
      color.r = lerp(0, color.r, brightness);
      color.g = lerp(0, color.g, brightness);
      color.b = lerp(0, color.b, brightness);
    }

    leds[_i].r = color.r;
    leds[_i].g = color.g;
    leds[_i].b = color.b;

    // run the last pattern.
    if (lastCommandTime > 0 && 
        crossfadeDuration > 0 &&
        currentTime < lastCommandTime + crossfadeDuration) {

      setColors(usePrevColors);

      int k2 = i2k(_i, prev_fixedLength);
      int j2 = prev_mapping(prev_frame, k);
      prev_color = prev_pattern(prev_frame, j2);
      if (prev_brightness < 1) {
        prev_color.r = lerp(0, prev_color.r, prev_brightness);
        prev_color.g = lerp(0, prev_color.g, prev_brightness);
        prev_color.b = lerp(0, prev_color.b, prev_brightness);
      }
      // float xfadePosition = map(currentTime, lastCommandTime, lastCommandTime + crossfadeDuration, 0, 1);

      xfadePosition = (currentTime - lastCommandTime) / (crossfadeDuration * 1.0);

      leds[_i].r = lerp(prev_color.r, leds[_i].r, xfadePosition);
      leds[_i].g = lerp(prev_color.g, leds[_i].g, xfadePosition);
      leds[_i].b = lerp(prev_color.b, leds[_i].b, xfadePosition);

      isFading = true;

       // Serial.println(isFading);
        // Serial.println(xfadePosition);
        // Serial.println("currentTime");
        // Serial.println(currentTime);
        // Serial.println("lastCommandTime");
        // Serial.println(lastCommandTime);
        // Serial.println("===================== ");

    }
    else{
      isFading = false;
      // Serial.println(isFading);
    }

       


  }



  // //SHORTER,BOTTOM ROW MIMICS TOP 'MAIN' ROW
  // // i wanna map i from 180-333 to 179-0
  // for (_i = OUTER_STRIP_LENGTH; _i < NUM_PIXELS; _i++) {
  //   leds[_i].r = leds[bottom_map[_i]].r;
  //   leds[_i].g = leds[bottom_map[_i]].g;
  //   leds[_i].b = leds[bottom_map[_i]].b;
  // }

    showAll();  


  if (frame >= MAX_FRAME) { 
    frame = 0;
  } 

  if (light)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
    
  light = !light;

}

int i2k(int i, bool fixedLength) {

  int k;

  if (fixedLength) {

    if (i < OUTER_STRIP_LENGTH) {
      k = map(i, 0, OUTER_STRIP_LENGTH - 2, 0, VIRTUAL_LENGTH - 1);
    } else { 
      k = map(i, OUTER_STRIP_LENGTH, NUM_PIXELS - 1, VIRTUAL_LENGTH - 1, 0);
    }

  } else { 

    if (i < OUTER_STRIP_LENGTH) {
      k = i;
    } else { 
      k = bottom_map[i];
    }

  }

  return k;

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


