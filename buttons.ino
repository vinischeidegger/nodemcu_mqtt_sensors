void initializeButtons(){
  //pinMode(buttonPin_RST, INPUT_PULLUP);
  //pinMode(buttonPin_SET, INPUT_PULLUP);
  pinMode(buttonPin_MID, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin_MID), okButtonPressed, FALLING);
  pinMode(buttonPin_RHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin_RHT), backButtonPressed, FALLING);
  pinMode(buttonPin_LFT, INPUT_PULLUP);
  pinMode(buttonPin_DWN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin_DWN), downButtonPressed, FALLING);
  pinMode(buttonPin_UP, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin_UP), upButtonPressed, FALLING);
}

unsigned long lastInterrupt;
int noiseRed = 300; // we set a 10ms no-interrupts window noise reduction
int selected = -1;
int entered = -1;

bool checkPush(int pinNumber) 
{
  int buttonPushed = digitalRead(pinNumber); 
  return buttonPushed == LOW;
}
//maxOptions = 5
//buffer = 0
ICACHE_RAM_ATTR void upButtonPressed() {
  long now = millis();
  if(now - lastInterrupt > noiseRed) 
  {    
    if (entered == -1 && selected < 6) {
      selected++;
      if(selected == maxOptions + buffer)
        buffer++;
    }
    lastInterrupt = now;
  }
}

ICACHE_RAM_ATTR void downButtonPressed() {
  long now = millis();
  if(now - lastInterrupt > noiseRed)
  {    
    if (entered == -1 && selected > -1) {
      if(selected == buffer && selected > 0)
        buffer--;
      selected--;
    }
    lastInterrupt = now;
  }
}

ICACHE_RAM_ATTR void okButtonPressed() {
  long now = millis();
  if(now - lastInterrupt > noiseRed)
  {    
    entered = selected;
    lastInterrupt = now;
  }
}

ICACHE_RAM_ATTR void backButtonPressed() {
  long now = millis();
  if(now - lastInterrupt > noiseRed)
  {    
    entered = -1;
    lastInterrupt = now;
  }
}
