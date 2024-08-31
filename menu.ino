void initializeDisplay(){
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

const char* mainMenuDesc = " MQTT Temp. Server ";
int buffer = 0;

void displaymenu(void) {

  //Serial.println( entered);
  const char *options[7] = { 
    " Main temperature ", 
    " Menu option 2 ", 
    " Menu option 3 ", 
    " Menu option 4 ",
    " Menu option 5 ",
    " Menu option 6 ",
    " Menu option 7 " 
  };
  int itemsQty = 6;
  int16_t yPos = 0; //16bits line
  if(entered == -1) {
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,yPos);
    if(temperatureC == -1)
      display.println("XX.XX C");
    else
      display.printf("%.2f C \n", temperatureC);
    display.println(mainMenuDesc);
    display.println("");
    for(int i = buffer;i < 5; i++) {
      if(i == selected) {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(options[i]);
      } else if(i != selected){
        display.setTextColor(SSD1306_WHITE);
        display.println(options[i]);
      }
    }
  } else {
        display.clearDisplay();
        display.setTextSize(1);             
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,yPos);
        display.println(mainMenuDesc);
        display.println(options[entered]);
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(2);
    if(entered == 0) {
        display.printf("\n%.2f C", temperatureC);
    } else if(entered == 1) {
        display.println("More cool stuff :)");
    }
  } 
  display.display();
}