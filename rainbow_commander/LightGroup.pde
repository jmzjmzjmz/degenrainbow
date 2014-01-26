float MASTER_BRIGHTNESS = 1.0;

class LightGroup {

  public final int address;

  // These are only public so cp5 can manipulate them!
  // PLZ Don't change manually! (use the setXxxx methods instead)
  public int pattern = OFF_PATTERN;
  public int mapping = 1;
  public int rate = 100;
  public int brightness = 127;
  public int frameOffset = 0;
  public int crossfadeDuration = 0;
  public boolean fixedLength = false;

  // Would have liked to make these arrays, but controlP5's reflection makes that harder.
  // Also addListener didn't work for controlGroups :(
  // DONT TOUCH
  public color color1 = color(255, 0, 0), color2 = color(0, 0, 255);

  public int r1, r2, g1, g2, b1, b2;

  // Interface elements
  public final ColorPicker colorPicker1, colorPicker2;
  public final RadioButton patternList;
  public final RadioButton mappingList;

  public final Slider rateSlider;
  public final Slider brightnessSlider;

  public final Slider frameOffsetSlider;
  public final Slider crossfadeDurationSlider;
  public final Toggle fixedLengthToggle;
  
  public final Bang bang;

  // Just the order in which we created this group
  protected final int index;

  public LightGroup(String title, int address) {

    index = lightGroups.size();
    lightGroups.add(this);
    this.address = address;

    // Build interface

    int SEND_MESSAGE_HEIGHT = 30;
    int x = index * LIGHT_GROUP_WIDTH + PADDING;
    int y = PADDING*2;

    bang = controlP5.addBang("bang-"+address)
             .setPosition(x, y)
             .setSize(LIGHT_GROUP_WIDTH-PADDING, SEND_MESSAGE_HEIGHT)
             .setTriggerEvent(Bang.RELEASE)
             .setLabel(title);

    y += SEND_MESSAGE_HEIGHT + PADDING*1.5;

    colorPicker1 = controlP5.addColorPicker("picker1-" + address)
                            .setPosition(x, y)
                            .setColorValue(color1);
    y += 70;

    colorPicker2 = controlP5.addColorPicker("picker2-" + address)
                            .setPosition(x, y)
                            .setColorValue(color2);
    y += 70;

    rateSlider = controlP5.addSlider("rate-"+address)
             .setPosition(x, y)
             .setRange(0, 127)
             .setSize(220, 20)
             .setDecimalPrecision(0)
             .setLabel("rate")
             .setValue(rate);

    y += PADDING * 2;

    brightnessSlider = controlP5.addSlider("brightness-"+address)
             .setPosition(x, y)
             .setRange(0, 127)
             .setSize(220, 20)
             .setDecimalPrecision(0)
             .setLabel("bright")
             .setValue(brightness);

    y += PADDING * 2;

    frameOffsetSlider = controlP5.addSlider("frame-offset-"+address)
             .setPosition(x, y)
             .setRange(0, 127)
             .setSize(220, 20)
             .setDecimalPrecision(0)
             .setLabel("f offset")
             .setValue(frameOffset);

    y += PADDING * 2;

    crossfadeDurationSlider = controlP5.addSlider("crossfade-duration-"+address)
             .setPosition(x, y)
             .setRange(0, 127)
             .setSize(220, 20)
             .setDecimalPrecision(0)
             .setLabel("xfade")
             .setValue(crossfadeDuration);

    fixedLengthToggle = controlP5.addToggle("fixed-length-"+address)
             .setPosition(x + 120, y + 200)
             .setLabel("fix len");

    y += PADDING * 2;
    
    patternList = controlP5.addRadioButton("patterns-" + address)
                           .setPosition(x , y)
                           .setSize(RADIO_SIZE, RADIO_SIZE)
                           .setNoneSelectedAllowed(false);

    int j = 0;
    for (int i = 0; i < patterns.length; i++) {
      if (patterns[i] != null) {
        patternList.addItem(patterns[i] + address, i);
        patternList.getItem(j).setCaptionLabel(patterns[i]);
        if (i == INITIAL_PATTERN) {
          patternList.activate(patternList.getItems().size()-1);
        }
        j++;
      }
    }

    mappingList = controlP5.addRadioButton("mappings-" + address)
                           .setPosition(x +150, y)
                           .setSize(RADIO_SIZE, RADIO_SIZE)
                           .setNoneSelectedAllowed(false);

    j = 0;
    for (int i = 0; i < mappings.length; i++) {
      if (mappings[i] != null) {
        mappingList.addItem(mappings[i] + address, i);
        mappingList.getItem(j).setCaptionLabel(mappings[i]);
        j++;
      }
    }

    mappingList.activate(0);
  
    


  }

  public LightGroupSettings getSettings() {

    return new LightGroupSettings(pattern, mapping, rate, brightness, color1, color2, frameOffset, crossfadeDuration, fixedLength);

  }

  public void applySettings(LightGroupSettings settings) {
    setRate(settings.rate);
    setBrightness(settings.brightness);
    setMapping(settings.mapping);
    setPattern(settings.pattern);
    setColor1(settings.color1);
    setColor2(settings.color2);
    setFrameOffset(settings.frameOffset);
    setCrossfadeDuration(settings.crossfadeDuration);
    setFixedLength(settings.fixedLength);
  }

  public void setRate(int value) {
    rateSlider.setValue(value);      
  }

  public void setBrightness(int value) {
    brightnessSlider.setValue(value);
  }

  public void setMapping(int value) {
    mappingList.activate(mappings[value] + address); // h8 u cp5
  }

  public void setPattern(int value) {
    patternList.activate(patterns[value] + address); // h8 u cp5
  }

  public void setColor1(int r, int g, int b) {
    r1 = r;
    g1 = g;
    b1 = b;
    colorPicker1.setColorValue(color(r1, g1, b1));
  }

  public void setColor2(int r, int g, int b) {
    r2 = r;
    g2 = g;
    b2 = b;
    colorPicker2.setColorValue(color(r2, g2, b2));
  }

  public void setColor1(color c) {
    setColor1((int)red(c), (int)green(c), (int)blue(c));
  }

  public void setColor2(color c) {
    setColor2((int)red(c), (int)green(c), (int)blue(c));
  }

  public void setFrameOffset(int value) {
    frameOffsetSlider.setValue(value);
  }

  public void setCrossfadeDuration(int value) {
    crossfadeDurationSlider.setValue(value);
  }

  public void setFixedLength(boolean value) {
    fixedLengthToggle.setValue(value);
  }

  public void sendMessage() {

    byte[] serialData = new byte[15];
    color color1 = colorPicker1.getColorValue();
    color color2 = colorPicker2.getColorValue();
    float a;

    serialData[0] = (byte)address;
    serialData[1] = (byte)rate;
    serialData[2] = (byte)pattern;
    serialData[3] = (byte)mapping;

    a = alpha(color1)/255;
    serialData[4] = (byte)(red(color1)   /2*a);
    serialData[5] = (byte)(green(color1) /2*a);
    serialData[6] = (byte)(blue(color1)  /2*a);

    a = alpha(color2)/255;
    serialData[7] = (byte)(red(color2)   /2*a);
    serialData[8] = (byte)(green(color2) /2*a);
    serialData[9] = (byte)(blue(color2)  /2*a);

    // Ignore third color.
    serialData[10] =  (byte)frameOffset;
    serialData[11] = (byte)crossfadeDuration;
    serialData[12] = (byte)(fixedLength ? 1 : 0);

    serialData[13] = (byte)(brightness * MASTER_BRIGHTNESS);
    serialData[14] = (byte)DELIMETER;



    for (int i = 0; i < serialData.length; i++) {
      messageQueue.offer(serialData[i]);
    }

  }

}
