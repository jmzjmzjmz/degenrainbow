int forward(long frame, int i) {
  return i;
}

int backward(long frame, int i) {
  return VIRTUAL_LENGTH - 1 - i;
}

int peak(long frame, int i) {
  if (i < VIRTUAL_LENGTH / 2) {
    return i;
  } else { 
    return VIRTUAL_LENGTH - 1 - i;
  }
}

int valley(long frame, int i) {
  if (i < VIRTUAL_LENGTH / 2) {
    return VIRTUAL_LENGTH / 2 - 1 - i;
  } else { 
    return i;
  }
}

int dither(long frame, int i) {
  if (i % 2 == 0) {
    return VIRTUAL_LENGTH - 1 - i;
  } else { 
    return i;
  }
}
