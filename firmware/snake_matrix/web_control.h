// =====================================================================
// web_control.h
// ---------------------------------------------------------------------
// The WiFi hotspot, the web page, and the live WebSocket connection to
// every phone or browser that opens it.
//
// Named web_control, not network: the ESP32 core has its own file
// called Network.h, and macOS treats network.h and Network.h as the
// same file name, which would confuse the compiler.
// =====================================================================

#pragma once

void webBegin();                        // start the hotspot and web server; call once in setup()
void webUpdate();                       // housekeeping; call every loop()
int webClientCount();                   // how many controllers are connected right now
void webSend(const char* message);      // send a text message to every controller
