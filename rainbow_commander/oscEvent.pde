interface OscReader {
  void read(OscMessage theOscMessage);
}

OscReader lemurReader = new LemurReader();
OscReader liveReader = new LiveReader();

void oscEvent(OscMessage theOscMessage) {

  if (DEBUG) println("[OSC] " + theOscMessage.addrPattern());

  lemurReader.read(theOscMessage);
  liveReader.read(theOscMessage);

}

void oscMessage(String addr, float value) {
  OscMessage message = new OscMessage(addr);
  message.add(value);
  oscP5.send(message, myRemoteLocation);
}


void setActiveAddr(Object[] activeAddr) {
  
  this.activeAddr = activeAddr;
}