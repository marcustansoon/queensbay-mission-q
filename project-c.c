// Source code reference:- https://create.arduino.cc/projecthub/munir03125344286/play-audio-with-arduino-df-player-mini-bb2a0e
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX 
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX 
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

// Create the Player object
DFRobotDFPlayerMini player;

// Input port configuration
static const uint8_t PIN_D12 = 12; // Play song when PIN_D12 is 0 V (grounded)
bool isMusicPlaying = false;
static const bool playOnce = true; // Indicate whether song can only be played once

void setup() {
  pinMode(PIN_D12, INPUT);           // Set pin to input
  digitalWrite(PIN_D12, HIGH);       // Turn on pullup resistors

  // Delay for 5 seconds
  delay(1000 * 5);

  // Init USB serial port for debugging
  Serial.begin(9600);
  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
   Serial.println("OK");
    // Set volume to maximum (0 to 30).
    player.volume(30);
    // Enable music loop
    // player.enableLoop();
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while(1);
  }
}

void loop() {
  if(digitalRead(PIN_D12) == LOW && !isMusicPlaying) {
    Serial.println("Playing song...");
    isMusicPlaying = true;
    // Play the first MP3 file on the SD card
    player.play(1);
  }
  if (player.available()) {
    printDetail(player.readType(), player.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      isMusicPlaying = playOnce;
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}
