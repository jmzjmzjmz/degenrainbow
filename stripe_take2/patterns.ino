struct CRGB solidColor(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  return color1;
}

struct CRGB colorWipe(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  if (pixelIndex < f % VIRTUAL_LENGTH-1)
    return color1;
  if (pixelIndex==VIRTUAL_LENGTH)
    return color1;
  
  return color2;
}

struct CRGB rainbow(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  return Wheel(f % 384);
}

// up/down variants to be replaced by mapping
struct CRGB rainbowCycle(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  int j = f % 384 * 5;      
  return Wheel((pixelIndex * 384 / VIRTUAL_LENGTH + j));
}

// up/down variants to be replaced by mapping
struct CRGB betterRainbowCycle(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  int j = f % 384 * 5;      
  return Wheel((pixelIndex * 384 * 3 / VIRTUAL_LENGTH + j));
}

struct CRGB colorAlternator(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  int s = 5;
  if (f % 2 == 0) 
    return color1;
  return color2;
}

struct CRGB stripe(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  int s = 4;
  if ((pixelIndex + f)/s % s < s/2)
    return color1;
  return color2;
}

struct CRGB bounce(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  int r = 5;
  float b = triangleWave(f, r) + (pixelIndex+f)/(r*2);
  b = b - floor(b);
  return lerpColor(color1, color2, b);
}
// this guy's not quite the same.
struct CRGB colorChase(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  int j = 9;
  if (pixelIndex >= f%VIRTUAL_LENGTH && pixelIndex <= f%VIRTUAL_LENGTH+j)
    return color1;
  return color2;
}

struct CRGB totesRandom(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
 if ((pixelIndex + f/10) % 2 == 0) 
   return (CRGB){(unsigned char)random(127), (unsigned char)random(127), (unsigned char)random(127)};
 return (CRGB){0, 0, 0};
}

struct CRGB gradient(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  float r = 25;
  return lerpColor(color1, color2, ((pixelIndex + f) % (int)r) / r);
}


//// THIS ONE MIGHT BE EFF'D : 

struct CRGB pulseSine(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  float r = 25;
  return lerpColor(color1, color2, (sin(f/r)+1)/2);
}

struct CRGB pulseSaw(long f, int pixelIndex) {
  if (f < 0) return NULL_COLOR;
  float r = 25;
  return lerpColor(color1, color2, (f % (int)r / r));
}



struct CRGB colorWipeMeter(long f, int pixelIndex) {
  
  if (f == -2) return NULL_COLOR;
    
  if (f == -1) {
    params[0] = random(0,VIRTUAL_LENGTH);
    return NULL_COLOR;
  }
  
  if (pixelIndex < params[0]) 
    return color1;
  return color2;
  
}

struct CRGB colorWipeMeterGradient(long f, int pixelIndex) {
  
  if (f == -2) return NULL_COLOR;
  
  if (f == -1) {
    params[0] = random(VIRTUAL_LENGTH);
    return NULL_COLOR;
  }

  if (pixelIndex < params[0]) 
    return lerpColor(color1, color2, (sin(f/25.0)+1)/2);
  return NULL_COLOR;

}

struct CRGB flickerStrobeTwo(long f, int pixelIndex) {
  
  if (f == -2) return NULL_COLOR;
  
  if (f == -1) {
    // Select two random pixels.
    params[0] = random(VIRTUAL_LENGTH);
    params[1] = random(VIRTUAL_LENGTH);
    return NULL_COLOR;
  }
  
  if (pixelIndex == (int)params[0]) {
    return color1;
  } else if (pixelIndex == (int)params[1]) {
    return color2;
  } else {
    return NULL_COLOR;
  }
  
}

struct CRGB flickerStrobeFour(long f, int pixelIndex) {
  
  if (f == -2) return NULL_COLOR;
  
  if (f == -1) {
    // Select four random pixels.
    params[0] = random(VIRTUAL_LENGTH);
    params[1] = random(VIRTUAL_LENGTH);
    params[2] = random(VIRTUAL_LENGTH);
    params[3] = random(VIRTUAL_LENGTH);
    return NULL_COLOR;
  }
  
  if (pixelIndex == (int)params[0] || 
      pixelIndex == (int)params[1]) {
    return color1;
  }
   else if (pixelIndex == (int)params[2] || 
             pixelIndex == (int)params[3]) {
  
    return color2;
  }
   else {
    return NULL_COLOR;
  }
  
}

struct CRGB pulseOnce(long f, int pixelIndex) {
  
  if (f == -2) {
    params[0] = 1.0;
    return NULL_COLOR;
  }
  
  if (f == -1) {
    params[0] *= 0.9;
    return NULL_COLOR;
  }
  
  return lerpColor(color2, color1, params[0]);
  
}
