// =====================================================================
// web_control.cpp
// ---------------------------------------------------------------------
// Two kinds of connection happen here:
//
//   1. HTTP: a browser asks for "/" and gets the controller page, once.
//   2. WebSocket: the page then opens a connection at "/ws" that stays
//      open. Button presses travel over it instantly in one direction,
//      score and game state in the other. No page reloads, no delay.
//
// Messages are short plain words, so they're easy to read and debug:
//   browser -> ESP32:  up  down  left  right  restart  pause
//   ESP32 -> browser:  state ready|playing|paused|over
//                      score 7
//                      reason hit a wall
// =====================================================================

#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "web_control.h"
#include "config.h"
#include "input.h"
#include "controller_page.h"   // the web page, stored as one long string

static AsyncWebServer server(80);    // port 80 is the standard web port
static AsyncWebSocket ws("/ws");     // the live connection lives at /ws
static unsigned long lastCleanup = 0;


// Turns one message from a controller into a game command.
static void handleMessage(const char* text) {
  if      (strcmp(text, "up") == 0)      inputPushDirection(DIR_UP);
  else if (strcmp(text, "down") == 0)    inputPushDirection(DIR_DOWN);
  else if (strcmp(text, "left") == 0)    inputPushDirection(DIR_LEFT);
  else if (strcmp(text, "right") == 0)   inputPushDirection(DIR_RIGHT);
  else if (strcmp(text, "restart") == 0) inputRequestRestart();
  else if (strcmp(text, "pause") == 0)   inputRequestPause();
  // anything else is ignored
}


// Called by the web server's background task whenever something happens
// on the WebSocket. We only care about incoming data here; connects and
// disconnects are noticed in the main loop by watching webClientCount().
static void onWsEvent(AsyncWebSocket* socket, AsyncWebSocketClient* client,
                      AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type != WS_EVT_DATA) {
    return;
  }

  // Only accept a complete message that arrived in one piece, as text.
  // Our page only ever sends short words, so that's all we expect.
  AwsFrameInfo* info = (AwsFrameInfo*)arg;
  bool wholeTextMessage = info->final && info->index == 0 &&
                          info->len == len && info->opcode == WS_TEXT;
  if (!wholeTextMessage) {
    return;
  }

  // The incoming bytes aren't a proper C string (no ending '\0'), so copy
  // them into a small buffer and add one. The size check means a long
  // message can never write past the end of the buffer.
  char text[16];
  size_t n = (len < sizeof(text) - 1) ? len : sizeof(text) - 1;
  memcpy(text, data, n);
  text[n] = '\0';

  handleMessage(text);
}


void webBegin() {
  // 1. Create the WiFi network.
  WiFi.mode(WIFI_AP);                       // AP = access point: we ARE the network
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  // Lower transmit power: plenty for one room, and it draws less current
  // in bursts, which matters on USB power with LEDs running too.
  WiFi.setTxPower(WIFI_POWER_8_5dBm);

  // 2. Give it a friendly name: http://snake.local
  if (MDNS.begin(HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
  }

  // 3. The live connection.
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // 4. The page itself. [](...) { ... } is a small unnamed function
  //    (a "lambda") that runs every time a browser asks for "/".
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", CONTROLLER_PAGE);   // 200 = "OK"
  });

  server.begin();

  Serial.println("WiFi hotspot ready:");
  Serial.printf("  Network:  %s\r\n", AP_SSID);
  Serial.printf("  Password: %s\r\n", AP_PASSWORD);
  Serial.printf("  Open:     http://%s.local  or  http://%s\r\n",
                HOSTNAME, WiFi.softAPIP().toString().c_str());
}


void webUpdate() {
  // Once a second, tidy up connections from phones that went away
  // without saying goodbye (screen locked, walked out of range).
  if (millis() - lastCleanup >= 1000) {
    lastCleanup = millis();
    ws.cleanupClients();
  }
}


int webClientCount() {
  return ws.count();
}


void webSend(const char* message) {
  ws.textAll(message);
}
