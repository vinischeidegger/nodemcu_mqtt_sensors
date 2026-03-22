/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";
/*
void handleIndex(AsyncWebServerRequest *request) {
  
}*/

/** Handle root or redirect to captive portal */
/*void handleRoot(AsyncWebServerRequest *request) {
  if (captivePortal(request)) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>HELLO WORLD!!</h1>"
  );
  if (server.client().localIP() == apIP) {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
  } else {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
  }
  server.sendContent(
    "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
    "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
/*boolean captivePortal(AsyncWebServerRequest *request) {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname)+".local")) {
    Serial.print("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}
*/
/** Wifi config page handler */
void handleWifi(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "-1");
    response->print(
      "<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>"
      "<h1>Wifi config</h1>"
    );
    if (request->client()->localIP() == apIP) {
      response->printf("<p>You are connected through the soft AP: %s</p>", softAP_ssid);
    } else {
      response->printf("<p>You are connected through the wifi network: %s</p>", ssid);
    }

    response->print("\r\n<br /><table><tr><th align='left'>SoftAP config</th></tr>");
    response->printf("<tr><td>SSID %s</td></tr>", softAP_ssid);
    response->printf("<tr><td>IP %s</td></tr>", toStringIp(WiFi.softAPIP()));
    response->print(
      "</table>"
      "\r\n<br />"
      "<table><tr><th align='left'>WLAN config</th></tr>"
    );
    response->printf("<tr><td>SSID %s</td></tr>", ssid);
    response->printf("<tr><td>IP %s</td></tr>", toStringIp(WiFi.localIP()));
    response->print(
      "</table>"
      "\r\n<br />"
      "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>"
    );
    Serial.println("scan start");
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        response->printf("\r\n<tr><td>SSID %s%s (%s)</td></tr>",
         WiFi.SSID(i), (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":" *", WiFi.RSSI(i));
      }
    } else {
      response->print("<tr><td>No WLAN found</td></tr>");
    }
    response->print(
      "</table>"
      "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
      "<input type='text' placeholder='network' name='n'/>"
      "<br /><input type='password' placeholder='password' name='p'/>"
      "<br /><input type='submit' value='Connect/Disconnect'/></form>"
      "<p>You may want to <a href='/'>return to the home page</a>.</p>"
      "</body></html>"
    );

    request->send(response);
}

/** Handle the WLAN save form and redirect to WLAN config page again *//*
void handleWifiSave(AsyncWebServerRequest *request) {
  Serial.println("wifi save");
  /*server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  server.arg("p").toCharArray(password, sizeof(password) - 1);*//*
  server.sendHeader("Location", "wifi", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  saveCredentials();
  connect = ssid.length() > 0; // Request WLAN connect with new credentials if there is a SSID
}

void handleNotFound(AsyncWebServerRequest *request) {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
}
*/
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler(ESP8266WiFiClass *wifi) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/index.html", "text/html");
    });
    server.on("/images/loading.gif", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/images/loading.gif", "image/gif");
    });
    server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
      String json = "[";
      int n = WiFi.scanComplete();
      if(n == -2){
        WiFi.scanNetworks(true);
      } else if(n){
        for (int i = 0; i < n; ++i){
          if(i) json += ",";
          json += "{";
          json += "\"rssi\":"+String(WiFi.RSSI(i));
          json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
          json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
          json += ",\"channel\":"+String(WiFi.channel(i));
          json += ",\"secure\":"+String(WiFi.encryptionType(i));
          json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
          json += "}";
        }
        WiFi.scanDelete();
        if(WiFi.scanComplete() == -2){
          WiFi.scanNetworks(true);
        }
      }
      json += "]";
      request->send(200, "text/json", json);
      json = String();
    });

/*
      server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "-1");
    response->print(
      "<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>"
      "<h1>Wifi config</h1>"
    );
    if (request->client()->localIP() == apIP) {
      response->printf("<p>You are connected through the soft AP: %s</p>", softAP_ssid);
    } else {
      response->printf("<p>You are connected through the wifi network: %s</p>", ssid);
    }

    response->print("\r\n<br /><table><tr><th align='left'>SoftAP config</th></tr>");
    response->printf("<tr><td>SSID %s</td></tr>", softAP_ssid);
    response->printf("<tr><td>IP %s</td></tr>", toStringIp(wifi->softAPIP()));
    response->print(
      "</table>"
      "\r\n<br />"
      "<table><tr><th align='left'>WLAN config</th></tr>"
    );
    response->printf("<tr><td>SSID %s</td></tr>", ssid);
    response->printf("<tr><td>IP %s</td></tr>", toStringIp(wifi->localIP()));
    response->print(
      "</table>"
      "\r\n<br />"
      "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>"
    );
    Serial.println("scan start");
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        response->printf("\r\n<tr><td>SSID %s%s (%s)</td></tr>",
         WiFi.SSID(i), (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":" *", WiFi.RSSI(i));
      }
    } else {
      response->print("<tr><td>No WLAN found</td></tr>");
    }
    response->print(
      "</table>"
      "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
      "<input type='text' placeholder='network' name='n'/>"
      "<br /><input type='password' placeholder='password' name='p'/>"
      "<br /><input type='submit' value='Connect/Disconnect'/></form>"
      "<p>You may want to <a href='/'>return to the home page</a>.</p>"
      "</body></html>"
    );

    request->send(response);
});*/
  }
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
    /*
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "-1");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is our captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    if (request->client()->localIP() == apIP) {
      response->printf("<p>You are connected through the soft AP: %s</p>", softAP_ssid);
    } else {
      response->printf("<p>You are connected through the wifi network: %s</p>", ssid);
    }
    response->printf(
      "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
      "</body></html>"
    );
    response->print("</body></html>");
    request->send(response);
    */
  }
};

void initializeServer(){
  server.addHandler(new CaptiveRequestHandler(&WiFi)).setFilter(ON_AP_FILTER);//only when requested from AP
  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
/*  server.on("/", HTTP_GET, handleRoot);
  server.on("/index", HTTP_GET, handleIndex);
  server.on("/wifisave", HTTP_POST, handleWifiSave);
  server.on("/generate_204", HTTP_GET, handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", HTTP_GET, handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.onNotFound ( handleNotFound );*/
  server.begin(); // Web server start
  Serial.println("HTTP server started");
}

