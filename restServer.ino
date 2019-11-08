#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* wifi_ssid = "YOURSSIDHERE";
const char* wifi_passwd = "WIFIKEYHERE";

char zeile1[16] = "";
char zeile2[16] = ""; 

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

int init_wifi() {
    int retries = 0;

    Serial.println("Connecting to WiFi AP..........");

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_passwd);
    // check the status of WiFi connection to be WL_CONNECTED
    while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
        retries++;
        delay(WIFI_RETRY_DELAY);
        Serial.print("#");
    }
    return WiFi.status(); // return the WiFi connection status
}


void getDisplay() {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& jsonObj = jsonBuffer.createObject();
    char JSONmessageBuffer[200];
    jsonObj["line1"] = zeile1;
    jsonObj["line2"] = zeile2;
    jsonObj.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    http_rest_server.send(200, "application/json", JSONmessageBuffer);
}
/*
void json_to_resource(JsonObject& jsonBody) {
    int id, gpio, status;

    id = jsonBody["id"];
    gpio = jsonBody["gpio"];
    status = jsonBody["status"];

    Serial.println(id);
    Serial.println(gpio);
    Serial.println(status);

    led_resource.id = id;
    led_resource.gpio = gpio;
    led_resource.status = status;
}

*/
void setDisplay() {
    StaticJsonBuffer<500> jsonBuffer;
    String post_body = http_rest_server.arg("body");
    Serial.println(post_body);

    JsonObject& jsonBody = jsonBuffer.parseObject(http_rest_server.arg("body"));

    Serial.print("HTTP Method: ");
    Serial.println(http_rest_server.method());
    
    if (!jsonBody.success()) {
        Serial.println("error in parsin json body");
        http_rest_server.send(400);
    }
    else {
          if (jsonBody["line1"].asString() != "" && jsonBody["line2"].asString() != "") {
              if(strlen(jsonBody["line1"].asString()) <= 16 && strlen(jsonBody["line2"].asString()) <= 16){
                strncpy(zeile1,jsonBody["line1"].asString(), strlen(jsonBody["line1"].asString()));
                strncpy(zeile2,jsonBody["line2"].asString(), strlen(jsonBody["line2"].asString()));
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print(zeile1);
                lcd.setCursor(0,1);
                lcd.print(zeile2);
                http_rest_server.sendHeader("Location", "/display");
                http_rest_server.send(201);
              }
              else{
                http_rest_server.send(500);
              }
          }
          else
            http_rest_server.send(404);
        
    }
}

void backlightOn() {
  lcd.backlight();
  http_rest_server.send(200);
}
void backlightOff() {
  lcd.noBacklight();
  http_rest_server.send(200);
}

void config_rest_server_routing() {
    http_rest_server.on("/", HTTP_GET, []() {
        http_rest_server.send(200, "text/html",
            "Welcome to the ESP8266 REST Web Server");
    });
    http_rest_server.on("/display", HTTP_GET, getDisplay);
    http_rest_server.on("/display", HTTP_POST, setDisplay);
    http_rest_server.on("/backlight/1", HTTP_GET, backlightOn);
    http_rest_server.on("/backlight/0", HTTP_GET, backlightOff);
}

void setup(void) {
    Serial.begin(115200);
    lcd.begin();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("REST-LCD");
    lcd.setCursor(0,1);
    lcd.print("loading...");

    if (init_wifi() == WL_CONNECTED) {
        Serial.print("Connected to ");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("WiFi connected!");
        Serial.print(wifi_ssid);
        Serial.print("--- IP: ");
        lcd.setCursor(0,1);
        lcd.print(WiFi.localIP());
        Serial.println(WiFi.localIP());
    }
    else {
        Serial.print("Error connecting to: ");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("WIFI ERROR");
        lcd.setCursor(0,1);
        lcd.print(wifi_ssid);
        Serial.println(wifi_ssid);
    }

    config_rest_server_routing();

    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop(void) {
    http_rest_server.handleClient();
}
