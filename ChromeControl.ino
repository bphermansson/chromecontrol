#include <RotaryEncoder.h>

// From Irlib2:s send sketch
#include <IRLibSendBase.h> 
#include <IRLib_P07_NECx.h>  
#include <IRLibCombo.h> 

// Timer
#include <elapsedMillis.h>
elapsedMillis sinceMovement;
elapsedMillis sinceTest1;
int t=0;
/*   
 From Irlib2:s record sketch:
 Samsung TV power - Received NECx Value:0xE0E040BF
 Vol+ 0xE0E0E01F
 Vol- 0xE0E0D02F
*/


// constants won't change. They're used here to set pin numbers:
const int gbuttonPin = 5;    // the number of the pushbutton pin
const int bbuttonPin = 4;    // the number of the pushbutton pin
const int pirPin = 6;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int gbuttonState;             // the current reading from the input pin
int glastButtonState = LOW;   // the previous reading from the input pin
int bbuttonState;             // the current reading from the input pin
int blastButtonState = LOW;   // the previous reading from the input pin
int pirState;
int screenState=0;    // 0 = monitor is off, 1 = on

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

RotaryEncoder encoder(9, 10);
IRsend mySender;

String inputString = "";      
boolean stringComplete = false; 

void setup() {
  Serial.begin(9600);
  
  pinMode(gbuttonPin, INPUT);
  pinMode(bbuttonPin, INPUT);
  pinMode(pirPin, INPUT);  
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, 1);
  delay(500);
  digitalWrite(ledPin, 0);
    

  inputString.reserve(200);

}

void loop() {
// Timer, time in milliseconds
    if (sinceMovement > 1800000) // "sinceMovement" auto-increases, 1800000 = 30 minutes
    {      
        sinceMovement = 0;
        t=0;
        Serial.println("Timeout");
        // x seconds has passed without movement. Turn off screen if it's on.
        if (screenState==1) {
          screenState=0;
          mySender.send(NECX,0xE0E040BF,0);
          Serial.println("Turn monitor off");
        }
    }

    if (sinceTest1 >= 1000) 
    {
        sinceTest1 = sinceTest1 - 1000;
        //Serial.println("Test1 (1 sec)");
        t++;
        //Serial.print(t);
        //Serial.print(" - ");
        //Serial.println(screenState);
    }

// Check for serial data
  if (stringComplete) {
    //Serial.println(inputString);
    inputString.trim();
    if (inputString == "V") {
      Serial.println("Version 0.1");
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
    digitalWrite(ledPin, 1);
    delay(500);
    digitalWrite(ledPin, 0);
  }
  
// Encoder
  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    //Serial.print(newPos);
    //Serial.println();
    if (pos < newPos) {
       Serial.println("u");
    } 
    else {
      Serial.println("d");
    }
    pos = newPos;
  }

// Buttons  
  int reading = digitalRead(gbuttonPin);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != glastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != gbuttonState) {
      gbuttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (gbuttonState == LOW) {
        //ledState = !ledState;
        //Serial.println("Green btn pressed");0x61a0f00f
        Serial.print("G");
        mySender.send(NECX,0xE0E040BF,0);
      }
    }
  }


  // save the reading. Next time through the loop, it'll be the lastButtonState:
  glastButtonState = reading;

  reading = digitalRead(bbuttonPin);
  if (reading != blastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != bbuttonState) {
      bbuttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (bbuttonState == LOW) {
        //Serial.println("Blue btn pressed");
        Serial.print("B");
        ledState = !ledState;
      }
    }
  }
  blastButtonState = reading;

// PIR sensor
  reading = digitalRead(pirPin);
  if (reading != pirState) {
    pirState = reading;
    if (pirState==1) {
       //Serial.println("Movement detected");
       Serial.print("P");
       //Serial.println(sinceMovement);

       // Turn screen on if it's off
       if (screenState==0) {
        screenState=1;
        mySender.send(NECX,0xE0E040BF,0);
        //Serial.println("Turn monitor on");
       }
       // Reset timer
       sinceMovement = 0;
       t=0;
    } 
    else {
       //Serial.println("Movement ended");
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
