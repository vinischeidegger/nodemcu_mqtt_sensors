const char* mainMenuDesc = "MQTT Temp. Server ";
int16_t minXpos;
int16_t iniXpos;
int16_t xPos = 0; //16bits line
bool scrollLeft = true;
const char *options[7] = { 
  " Main temperature ", 
  " WiFi Status      ", 
  " Config via AP    ", 
  " Menu option 4    ",
  " Menu option 5    ",
  " Menu option 6    ",
  " Menu option 7    " 
};

void initializeDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 1 seconds
  display.setTextWrap(false);
  // Clear the buffer
  display.clearDisplay();
  iniXpos = 48;
  xPos = iniXpos;
}

void displayMainMenu(int yPos){
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0,yPos);
  if(temperatureC == -1)
    display.print("XX.XX");
  else
    display.printf("%.2f", temperatureC);
  xPos = 6 * 6;  
  display.setCursor(xPos,yPos);
  display.print("C\n");
  /*yPos = yPos + 10;
  display.setCursor(0,yPos);*/
  display.print(mainMenuDesc);
  yPos = yPos + 20;
  display.setCursor(0,yPos);
  int maxMenu = maxOptions + buffer;
  for(int i = buffer; i < maxMenu; i++) {
    if(i == selected) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.println(options[i]);
    } else if(i != selected){
      display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      display.println(options[i]);
    }
  }
}

uint dotCount = 1;
const long scrollInterval = 200;        // Interval at which to publish sensor readings
unsigned long previousScrollMillis = 0;   // Stores last time temperature was published
String dots = "";

void displaymenu(void) {
  int itemsQty = 6;
  int16_t yPos = 0; //16bits line
  if(entered == -1) {
    displayMainMenu(yPos);
  } else {
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(0,yPos);
    display.println(mainMenuDesc);
    display.println(options[entered]);
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    if(entered == 0) {
      display.setTextSize(2);
      display.printf("\n%.1fC %.1fC", temperatureC, temperatureC1);
    } else if(entered == 1) {
      // Wifi Settings
      int wifiStatus = WiFi.status();
      String statusText = "";
      switch(wifiStatus) {
        case WL_CONNECTED :
          statusText = "CONNECTED";
          break;
        default :
          statusText = "DISCONNECTED";
          break;
      }
      display.println("");
      yPos = 24;
      scrollText(ssid, yPos);
      display.setCursor(0, yPos);
      display.print("SSID:   ");
      display.setCursor(0, yPos+8);
      display.printf("Status: %s\n", statusText.c_str());
      display.printf("IP:     %s\n", ipAddress.c_str());
    } else if(entered == 2) {
      display.println("\nConnect your phone to");
      display.setTextSize(2);
      display.printf("  %s\n", softAP_ssid);
      uint max = 11;
      if(WiFi.getMode()==WIFI_STA) {
        display.print("Creating Access Point");
        apConfig();
      } else {
        int numClients = WiFi.softAPgetStationNum();
        if (numClients==0){
          unsigned long currentMillis = millis();
          if (currentMillis - previousScrollMillis >= (scrollInterval * 5)) {
            // Save the last time a new reading was published
            previousScrollMillis = currentMillis;
            dots = "";
            for(int i = 1; i<=dotCount; i++)
              dots += ".";
            dotCount = (dotCount==max)?1:dotCount+1;
          }
          display.printf("%s", dots);
        } else {
          display.print("Connected!");
        }
        handleAp();
      }
    }
  } 
  display.display();
}

void scrollText(String text, int16_t yPos){
  minXpos = iniXpos - text.length();
  display.setCursor(xPos, yPos);
  unsigned long currentMillis = millis();
  if (currentMillis - previousScrollMillis >= scrollInterval) {
    // Save the last time a new reading was published
    previousScrollMillis = currentMillis;
    if(scrollLeft && xPos > minXpos)
      xPos--;
    else if (xPos == minXpos)
      scrollLeft = false;
    if(!scrollLeft && xPos < iniXpos)
      xPos++;
    else if (xPos == iniXpos)
      scrollLeft = true;
  }
  display.printf("%s", ssid.c_str());
}