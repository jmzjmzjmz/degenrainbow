/*
TAKES IN SERIAL DATA, SIMPLIFIES IT AND SPITS IT OUT VIA SPI RF MODULE
*/

 // make an empty tab called ignore.h and this will run.
#include "ignore.h" 

//#include "LPD8806.h"
//LPD8806 strip = LPD8806(8);
#define NUM_PIXELS 10

/*
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 8
 * CSN -> 7
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#define PACKET_ATTEMPTS 1
int MY_PAYLOAD = 3*NUM_PIXELS;

byte fullStripData[3*NUM_PIXELS];
byte fullStripHIDE[3*NUM_PIXELS];

//#include <Adafruit_NeoPixel.h>
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(116, 6, NEO_GRB + NEO_KHZ400);


boolean light = false;

/*

TODO: DEFINE PROPER ADDRESS HERE AND IN UFO COMMANDER

*/
#define myADDRESS 6 
#define mySETADDRESS 1
#define globalADDR 0


#define NULL_PATTERN 0
#define OFF_PATTERN 68
#define PAUSE_PATTERN 67
#define ADV_PATTERN 101
#define RESET_FRAME 100

#define TIMING_ADDR 100

#define MAX_FRAME 100000

unsigned int incomingBrightness=0;
unsigned int incomingRate=0;
unsigned int rate = 20;
unsigned int patternByte = NULL_PATTERN;

// unix timestamp that the sketch starts at
unsigned long startedAt = 0;
unsigned long lastTime = -1;

float brightness = 1.0;
int r1 = 255, g1 = 255, b1 = 255, 
r2 = 0, g2 = 0, b2 = 0, 
r3 = 0, g3 = 0, b3 = 0;

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


uint8_t txAddr[5];

void setup() {  
    for(int j = 0; j < 5; j++){ 
  
  txAddr[j] = 0xE7;
  
  }
  for(int j = 0;j< NUM_PIXELS; j++){
    fullStripHIDE[j]=0;
  }
  
  pinMode(13, OUTPUT); 

//  Serial1.begin(9600); 
  Serial.begin(9600);
  
  
  
  //MIRF CONFIG
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
//  Mirf.setRADDR(txAddr);
Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = MY_PAYLOAD;   
  Mirf.channel = 90;
  Mirf.configRegister(RF_SETUP,0x27);
  Mirf.configRegister(EN_AA,0x00);  // TESTING DISABLING AUTOACK
  Mirf.configRegister(CONFIG,0x0C);
 Mirf.setTADDR(txAddr);
//Mirf.setTADDR((byte *)"serv1");
  Mirf.config();
//  Mirf.setTADDR((byte *)"serv1");
 
  
//  strip.begin();
  
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

  pattern = &pulseOnce;
  pattern(-2, 0);

  startedAt = 0;//RTC.now().unixtime();

  inputString.reserve(200);

}

void read() {
  
  while (Serial.available()) {


    char c = (char)Serial.read();
    // Serial.println(c, DEC);
    inputString += c;
    if (c == -128) {


      
      if (inputString.charAt(0) == -127) {

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
          
          rate = (unsigned char)inputString.charAt(1) + 1;
          patternByte = (unsigned char)inputString.charAt(2);

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

  color1 = Color(r1, g1, b1);
  color2 = Color(r2, g2, b2);
  color3 = Color(r3, g3, b3);

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

  for (int i = 0; i < NUM_PIXELS; i++) {

    int j = mapping(frame, i);
    uint32_t color = pattern(frame, j);


    uint8_t r = red(color), g = green(color), b = blue(color);


    if (brightness < 1) {
      r = lerp(0, r, brightness);
      g = lerp(0, g, brightness);
      b = lerp(0, b, brightness);
    }

   // strip.setPixelColor(i, r, g, b);
   
   //fill data array with rgb values
   fullStripData[i*3] = r;
   fullStripData[i*3+1] = g;
   fullStripData[i*3+2] = b;
   
//   if(i == 0){
//    sendMirfPackets(r,g,b);
//   }

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
sendMirfPackets(fullStripData);
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


void hideAll() {
  sendMirfPackets(fullStripHIDE);

//  sendMirfPackets(0,0,0);
}

void showAll(){
//  strip.show();    
}

void sendMirfPackets(uint8_t red, uint8_t green, uint8_t blue){
  
  for(int packetCount = 0; packetCount < PACKET_ATTEMPTS; packetCount++){
 
    Mirf.send((byte *)&red);
  Mirf.send((byte *)&green);
  Mirf.send((byte *)&blue);
  while(Mirf.isSending()){
  } 
  
//  Serial.println("I'M SENDING: ");
//  Serial.println(red);
//   Serial.println(green);
//    Serial.println(blue);
  
}
}

int counter = 0;

void sendMirfPackets(byte myData[])
{
  Mirf.config();
  //Mirf.send(myData);
  Mirf.SendWithRetransmit(myData,10);
  /*
  for(int packetCount = 0; packetCount < PACKET_ATTEMPTS; packetCount++)
  {
      

    
//    Serial.println("I Sent: ");
//    for(int i = 0; i < sizeof(myData); i++){
//    Serial.println(myData[i]);
//    }
//    Serial.println(" ");
  }
  */
  while(Mirf.isSending())
  {
  } 
}

// Convert separate R,G,B into combined 32-bit GRB color:
uint32_t Color(byte r, byte g, byte b) {
  return ((uint32_t)(g | 0x80) << 16) |
         ((uint32_t)(r | 0x80) <<  8) |
                     b | 0x80 ;
}

