/*******************************************************************************
 *    ___ _                                ___                   _ 
 *   / __| |_ _ _ __ _ _ _  __ _ ___ _ _  | _ ) ___  __ _ _ _ __| |
 *   \__ \  _| '_/ _` | ' \/ _` / -_) '_| | _ \/ _ \/ _` | '_/ _` |
 *   |___/\__|_| \__,_|_||_\__, \___|_|   |___/\___/\__,_|_| \__,_|
 *                         |___/                                   
 *                         
 *    ___ ___ ___ ___ ___   ___ __    __ _                              
 *   | __/ __| _ ( _ )_  )/ /  / /   / _(_)_ _ _ ____ __ ____ _ _ _ ___ 
 *   | _|\__ \  _/ _ \/ // _ \/ _ \ |  _| | '_| '  \ V  V / _` | '_/ -_)
 *   |___|___/_| \___/___\___/\___/ |_| |_|_| |_|_|_\_/\_/\__,_|_| \___
 *                                                       
 * 
 *******************************************************************************
 *  Dajovic 2019 - danjovic@hotmail.com
 *   
 *  This code is released under GPL 
 *******************************************************************************
 *   
 * thanks circuits4you.com for tutorials on esp8266
 */
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "index.h" //HTML webpage contents

String webPage,phrase;

//SSID and Password of your WiFi router
const char* ssid = "StrangerBoard";
const char* password = "12345678";

ESP8266WebServer server(80); //Server on port 80

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 webPage = MAIN_page; //Read HTML contents
 server.send(200, "text/html", webPage); //Send web page
}

void handleMessage()
{
 char buf[256];
 webPage = MAIN_page;
 phrase=server.arg("myPhrase");
// Serial.println("Text Received, contents:");
 Serial.println(phrase);
 //phrase.toCharArray(buf, sizeof(buf));
 //Serial.print(buf);
 server.send(200,"text/html",webPage);
}



//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(9600);
  
  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpot IP:");
  Serial.println(myIP);
 
  server.on("/", handleRoot);      //Which routine to handle at root location

  server.on("/sendMessage",handleMessage);  // receive data 

  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  server.handleClient();          //Handle client requests
}
