int forward(long frame, int i) {
  return i;
}

int backward(long frame, int i) {
  return NUM_PIXELS - 1 - i;
}

int peak(long frame, int i) {
  if (i < NUM_PIXELS / 2) {
    return i;
  } else { 
    return NUM_PIXELS - 1 - i;
  }
}

int valley(long frame, int i) {
  if (i < NUM_PIXELS / 2) {
    return NUM_PIXELS / 2 - 1 - i;
  } else { 
    return i;
  }
}

int dither(long frame, int i) {
  if (i % 2 == 0) {
    return NUM_PIXELS - 1 - i;
  } else { 
    return i;
  }
}
