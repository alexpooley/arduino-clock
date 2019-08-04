#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define SERIAL_BAUD 115200	//make sure this is the same in arduino.ino


ESP8266WebServer server(80);
bool led_on = true;

void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

//unravel this code to figure out how to change it in your code
String html_page = "<!DOCTYPE html><html><head><script>window.onload = function(){setInterval(function(){ var x = new XMLHttpRequest(); x.onreadystatechange = function(){if(this.readyState == 4 && this.status == 200){document.getElementById('reading').textContent = x.resp  bool data_in = false;onseText;} }; x.open(\"GET\",\"/reading\",true); x.send();},500);document.getElementById('button').onclick = function(){var x = new XMLHttpRequest(); x.open('GET','/command',true); x.send();}}</script></head><body><button id='button'> Toggle Led 13</button><h1>Latest data from arduino:</h1><span id='reading'></span></body></html>";

String serialData = "No data yet..</br>...";

IPAddress ip(10, 0, 0, 7);

void setup(void) {
	
	Serial.begin(SERIAL_BAUD); 
	WiFi.softAPConfig(ip, ip, IPAddress(255,255,255,0));
	WiFi.softAP("XC44X1 Dual Boards");

	server.on("/", [](){				//send html code, from above
		server.send(200, "text/html", html_page);
	});

	server.on("/reading",[](){			//send raw serial data
		server.send(200, "text/plain", serialData);
	});

  server.on("/command",[](){
    
    if(led_on){
      Serial.println("~LEDON|"); //send a command to turn a led on to the arduino
    }else{
      Serial.println("~LEDOFF|"); //send a command to turn a led off
    }

    led_on = ! led_on;
  });

	server.onNotFound(handleNotFound);
	server.begin();

	delay(1000);
	Serial.println();
	Serial.println("server Started!");

	//it is usually useful to have a "special char" to signify a new string in these situations
	//so we will tell the arduino we are ready with using the ~ and | as special symbols. check our arduino.ino
	Serial.println("~OK|");
}

void loop(void) {

	while(Serial.available()){
		char c = Serial.read();

		if ( c == '\n'){
			serialData += " \n "; //this is not good practise
		}
		else
			serialData += c;
	}

  
	server.handleClient();
}
