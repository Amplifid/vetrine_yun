//  control radio 220V. plugs, modified for Arduino

//  derived from: http://arduino.cc/en/Guide/ArduinoYun#toc19

//  include Fastled library Cylon example


/*
  Arduino Yún Bridge example

 This example for the Arduino Yún shows how to use the
 Bridge library to access the digital and analog pins
 on the board through REST calls. It demonstrates how
 you can create your own API when using REST style
 calls through the browser.

 Possible commands created in this shetch:
 
 *Me-> "http://vetrine.local/arduino/neo/2"    -> 4 = numero delle reiterazioni

 *Me-> "http://vetrine.local/arduino/luce/4"   -> D-On
 *Me-> "http://vetrine.local/arduino/luce/11"  -> D-Off

 * "/arduino/digital/13"     -> digitalRead(13)
 * "/arduino/digital/13/1"   -> digitalWrite(13, HIGH)
 * "/arduino/analog/2/123"   -> analogWrite(2, 123)
 * "/arduino/analog/2"       -> analogRead(2)
 * "/arduino/mode/13/input"  -> pinMode(13, INPUT)
 * "/arduino/mode/13/output" -> pinMode(13, OUTPUT)

 This example code is part of the public domain

 http://arduino.cc/en/Tutorial/Bridge

 */

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

#include "FastLED.h"

#define switchPin1 4     // D-On
#define switchPin2 5     // C-On
#define switchPin3 6     // B-On
#define switchPin4 7     // A-On

#define switchPin5 8     // A-Off
#define switchPin6 9     // B-Off
#define switchPin7 10    // C-Off
#define switchPin8 11    // D-Off

// How many leds in your strip?
#define NUM_LEDS 12

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 3
//  #define CLOCK_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];


// Listen on default port 5555, the webserver on the Yún
// will forward there all the HTTP requests for us.
YunServer server;

void setup() {
  pinMode(switchPin1, OUTPUT);
  pinMode(switchPin2, OUTPUT);
  pinMode(switchPin3, OUTPUT);
  pinMode(switchPin4, OUTPUT);
  pinMode(switchPin5, OUTPUT);
  pinMode(switchPin6, OUTPUT);
  pinMode(switchPin7, OUTPUT);
  pinMode(switchPin8, OUTPUT);

  Serial.begin(9600);
  
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  
  	FastLED.addLeds<NEOPIXEL,DATA_PIN>(leds, NUM_LEDS);

}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "neo" command?
  if (command == "neo") {
      client.print(F("neo"));
      neoCommand(client);
  }

  // is "luce" command?
  if (command == "luce") {
    luceCommand(client);
  }

  // is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
  }

  // is "analog" command?
  if (command == "analog") {
    analogCommand(client);
  }

  // is "mode" command?
  if (command == "mode") {
    modeCommand(client);
  }
}



void cylon() { 
          Serial.print(F("CYLON"));

	// First slide the led in one direction
	for(int i = 0; i < NUM_LEDS; i++) {
		// Set the i'th led to red 
		leds[i] = CRGB::Red;
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		leds[i] = CRGB::Black;
		// Wait a little bit before we loop around and do it again
		delay(100);
	}

	// Now go in the other direction.  
	for(int i = NUM_LEDS-1; i >= 0; i--) {
		// Set the i'th led to red 
		leds[i] = CRGB::Red;
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		leds[i] = CRGB::Black;
		// Wait a little bit before we loop around and do it again
		delay(100);
	}
}


void neoCommand(YunClient client) {
  int upto;

  // Read pin number
  upto = client.parseInt();

    for(int i = 0; i < upto; i++) {
    cylon();
      
//    client.print(F("neo"));
//    client.print(upto);

    delay(1000);
 
   }
 
  // Send feedback to client
  client.print(F("Neopixel"));
  
  // Update datastore key with the current pin value
  String key = "N";
  key += 1;
  Bridge.put(key, String(HIGH));
}


void luceCommand(YunClient client) {
  int pin;

  // Read pin number
  pin = client.parseInt();
  digitalWrite(pin, HIGH);
  delay(1000);
  digitalWrite(pin, LOW);

  // Send feedback to client
  client.print(F("Pin Luce "));
  client.print(pin);

  // Update datastore key with the current pin value
  String key = "L";
  key += pin;
  Bridge.put(key, String(HIGH));
}








void digitalCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  }
  else {
    value = digitalRead(pin);
  }

  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void analogCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (client.read() == '/') {
    // Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    // Read analog pin
    value = analogRead(pin);

    // Send feedback to client
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void modeCommand(YunClient client) {
  int pin;

  // Read pin number
  pin = client.parseInt();

  // If the next character is not a '/' we have a malformed URL
  if (client.read() != '/') {
    client.println(F("error"));
    return;
  }

  String mode = client.readStringUntil('\r');

  if (mode == "input") {
    pinMode(pin, INPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }

  if (mode == "output") {
    pinMode(pin, OUTPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as OUTPUT!"));
    return;
  }

  client.print(F("error: invalid mode "));
  client.print(mode);
}


