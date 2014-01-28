class LiveReader implements OscReader {

  void read(OscMessage theOscMessage) {

    String p = theOscMessage.addrPattern();

    Matcher presets = Pattern.compile("/preset/(\\d+)").matcher(p);
    Matcher patterns = Pattern.compile("/pattern/(\\d+)").matcher(p);
    Matcher mappings = Pattern.compile("/mapping/(\\d+)").matcher(p);
    Matcher address = Pattern.compile("/address/(\\d+)").matcher(p);
    
    if (presets.find() && theOscMessage.get(0).intValue() == 1) {
      
      applyPreset(parseInt(presets.group(1)));
      sendFeedback();
      
    } else if (patterns.find() && theOscMessage.get(0).intValue() == 1) {
      
      for (int j = 0; j < activeAddr.length; j++) {
        if (activeAddr[j].equals(1.0)) {
          LightGroup l0 = (LightGroup)lightGroups.get(0);
          l0.setPattern(patternsToIndeces[parseInt(patterns.group(1))]);
//          l0.sendMessage();
          LightGroup l = (LightGroup)lightGroups.get(1);
          l.setPattern(patternsToIndeces[parseInt(patterns.group(1))]);
          l.sendMessage();
          LightGroup l2 = (LightGroup)lightGroups.get(2);
          l2.setPattern(patternsToIndeces[parseInt(patterns.group(1))]);
          l2.sendMessage();
          LightGroup l3 = (LightGroup)lightGroups.get(3);
          l3.setPattern(patternsToIndeces[parseInt(patterns.group(1))]);
          l3.sendMessage();
          LightGroup l4 = (LightGroup)lightGroups.get(4);
          l4.setPattern(patternsToIndeces[parseInt(patterns.group(1))]);
          l4.sendMessage();

        }
      }

    } else if (mappings.find() && theOscMessage.get(0).intValue() == 1) {
      
      for (int j = 0; j < activeAddr.length; j++) {
        if (activeAddr[j].equals(1.0)) {
          
          LightGroup l = (LightGroup)lightGroups.get(1);
          l.setMapping(parseInt(mappings.group(1))+1); // hack.
          l.sendMessage();
          LightGroup l1 = (LightGroup)lightGroups.get(2);
          l1.setMapping(parseInt(mappings.group(1))+1); // hack.
          l1.sendMessage();
          LightGroup l2 = (LightGroup)lightGroups.get(3);
          l2.setMapping(parseInt(mappings.group(1))+1); // hack.
          l2.sendMessage();
          LightGroup l3 = (LightGroup)lightGroups.get(4);
          l3.setMapping(parseInt(mappings.group(1))+1); // hack.
          l3.sendMessage();




        }
      }

    } else if (address.find() && theOscMessage.get(0).intValue() == 1) { 

      int a = parseInt(address.group(1));
      for (int i = 0; i < activeAddr.length; i++) {
        if (a == i) {
          activeAddr[i] = 1.0;
        } else {
          activeAddr[i] = 0.0;          
        }
      }
      
    } else if (theOscMessage.addrPattern().equals("/stealth/x")) { 


      if (theOscMessage.get(0).intValue() == 1) {
      
        stealth = true;
        
      
      } else { 
        
        stealth = false;

        for (int j = 0; j < activeAddr.length; j++) {
          if (activeAddr[j].equals(1.0)) {
            LightGroup l = (LightGroup)lightGroups.get(1);
            l.sendMessage();
             LightGroup l2 = (LightGroup)lightGroups.get(2);
            l2.sendMessage();
             LightGroup l3 = (LightGroup)lightGroups.get(3);
            l3.sendMessage();
             LightGroup l4 = (LightGroup)lightGroups.get(4);
            l4.sendMessage();
          }
        }

      }

    } else if (theOscMessage.addrPattern().equals("/rfport/on")) { 

      rfPortActive = true;
      rfPortActiveToggle.setValue(rfPortActive);

    } else if (theOscMessage.addrPattern().equals("/rfport/off")) { 

      rfPortActive = false;
      rfPortActiveToggle.setValue(rfPortActive);  

    } else if (p.equals("/red1")) { 
  
      for (int i = 0; i < activeAddr.length; i++) {
        
        if (activeAddr[i].equals(1.0)) {
          int v = (int)(theOscMessage.get(0).floatValue() * 255);
          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setColor1(v, l.g1, l.b1);
          l.sendMessage();
        }
        
      }

    } else if (p.equals("/green1")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 255);
          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setColor1(l.r1, v, l.b1);
          l.sendMessage();

        }
      }

    } else if (p.equals("/blue1")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 255);
          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setColor1(l.r1, l.g1, v);
          l.sendMessage();


        }
      }

    } else if (p.equals("/red2")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 255);
          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setColor2(v, l.g2, l.b2);
          l.sendMessage();


        }
      }

    } else if (p.equals("/green2")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 255);
          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setColor2(l.r2, v, l.b2);
          l.sendMessage();


        }
      }

    } else if (p.equals("/blue2")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 255);
          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setColor2(l.r2, l.g2, v);
          l.sendMessage();


        }
      }

    } else if (p.equals("/rate")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 127);
          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setRate(v);
          l.sendMessage();

        }
      }

    } else if (p.equals("/brightness")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 127);
          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setBrightness(v);
          l.sendMessage();

        }
      }

    } else if (p.equals("/crossfade")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 127);

          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setCrossfadeDuration(v);
          l.sendMessage();

        }
      }

    } else if (p.equals("/offset")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 127);

          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setFrameOffset(v);
          l.sendMessage();

        }
      }

    } else if (p.equals("/stretch")) { 

      for (int i = 0; i < activeAddr.length; i++) {
        if (activeAddr[i].equals(1.0)) {

          int v = (int)(theOscMessage.get(0).floatValue() * 127);

          LightGroup l = (LightGroup)lightGroups.get(i);
          l.setFixedLength(v > 0);
          l.sendMessage();

        }
      }

    }
    
  }
 
   void sendFeedback() {

    int feedbackIndex = 0;

    for (int i = 0; i < activeAddr.length; i++) {
      if (activeAddr[i].equals(1.0)) {
        feedbackIndex = i;
        break;
      }
    }

    LightGroup l = (LightGroup)lightGroups.get(feedbackIndex);

    oscMessage("/RedSlider1/x",   map(red(l.color1), 0, 255, 0, 1));
    oscMessage("/GreenSlider1/x", map(green(l.color1), 0, 255, 0, 1));
    oscMessage("/BlueSlider1/x",  map(blue(l.color1), 0, 255, 0, 1));

    oscMessage("/RedSlider2/x",   map(red(l.color2), 0, 255, 0, 1));
    oscMessage("/GreenSlider2/x", map(green(l.color2), 0, 255, 0, 1));
    oscMessage("/BlueSlider2/x",  map(blue(l.color2), 0, 255, 0, 1));

    oscMessage("/RateSlider/x",   map(l.rate, 0, 127, 0, 1));
    oscMessage("/BrightnessSlider/x", map(l.brightness, 0, 127, 0, 1));

//    // Send pattern message.
//    
//    int patternIndex = 0;
//    for (int i = 0; i < patternsToIndeces.length; i++) {
//      if (patternsToIndeces[i] == l.pattern) {
//        patternIndex = i;
//      }
//    }
//
//    float[] patternMessage = new float[patternsToIndeces.length];
//    for (int i = 0; i < patternMessage.length; i++) {
//      if (i == patternIndex) {
//        patternMessage[i] = 1.0;
//      }
//    }
//
//    OscMessage message = new OscMessage("/Patterns/x");
//    message.add(patternMessage);
//    oscP5.send(message, myRemoteLocation);
//
//    // Send mapping message.
//
//    int mappingIndex = l.mapping - 1; // HACK
//    message = new OscMessage("/Mappings/x");
//    float[] mappingMessage = new float[mappings.length];
//    for (int i = 0; i < mappings.length; i++) {
//      if (i == mappingIndex) {
//        mappingMessage[i] = 1.0;
//      }
//    }
//    message.add(mappingMessage);
//    oscP5.send(message, myRemoteLocation);

  }

}
