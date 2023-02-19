// Source code reference:- https://create.arduino.cc/projecthub/munir03125344286/play-audio-with-arduino-df-player-mini-bb2a0e
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2;  // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3;  // Connects to module's TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

// Create the Player object
DFRobotDFPlayerMini player;

// Input port configuration
static const uint8_t PIN_D12 = 12;  // Play song when PIN_D12 is 0 V (grounded)

// State machine states
enum STATES {
  IDLE,
  PLAY_BACKGROUND_AUDIO_1,
  WAIT_SIREN_TRIGGER,
  PLAY_SIREN_AUDIO,
  WAIT_SIREN_AUDIO_FINISHED,
  PLAY_BACKGROUND_AUDIO_2,
  WAIT_BACKGROUND_AUDIO_FINISHED
};
enum STATES currentState = IDLE, nextState = IDLE;

// Audio file numbering in SD card
enum AUDIO {
  BACKGROUND_AUDIO = 1,
  SIREN_AUDIO = 3,
  ROOM2_AUDIO = 2  
};
enum AUDIO currentMusic = BACKGROUND_AUDIO;

// Indicate whether a music is currently playing
bool isMusicPlaying = false;

void setup() {
  // Set pin to input and pullup by default
  pinMode(PIN_D12, INPUT_PULLUP);

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
    player.volume(1);
    // Enable music loop
    // player.enableLoop();
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while (1)
      ;
  }
}

void loop() {
  // Set next state
  currentState = nextState;

  // SM transition definition
  switch (currentState) {
    case IDLE:
      nextState = PLAY_BACKGROUND_AUDIO_1;
      break;
    case PLAY_BACKGROUND_AUDIO_1:
      nextState = WAIT_SIREN_TRIGGER;
      break;
    case WAIT_SIREN_TRIGGER:
      // Check if siren button is triggered (active LOW)
      if (digitalRead(PIN_D12) == LOW)
        nextState = PLAY_SIREN_AUDIO;
      // Repeat background music
      else if (!isMusicPlaying)
        nextState = PLAY_BACKGROUND_AUDIO_1;
      break;
    case PLAY_SIREN_AUDIO:
      nextState = WAIT_SIREN_AUDIO_FINISHED;
      break;
    case WAIT_SIREN_AUDIO_FINISHED:
      // When siren audio is finished playing, play bg music
      if (!isMusicPlaying)
        nextState = PLAY_BACKGROUND_AUDIO_2;
      else
        nextState = WAIT_SIREN_AUDIO_FINISHED;
      break;
    case PLAY_BACKGROUND_AUDIO_2:
      nextState = WAIT_BACKGROUND_AUDIO_FINISHED;
      break;
    case WAIT_BACKGROUND_AUDIO_FINISHED:
      // When bg music is finished playing, repeat the bg music
      if (!isMusicPlaying)
        nextState = PLAY_BACKGROUND_AUDIO_2;
      else
        nextState = WAIT_BACKGROUND_AUDIO_FINISHED;
      break;
    default:
      nextState = IDLE;
      break;
  }

  // SM actions definition
  switch (currentState) {
    case IDLE:
      break;
    case PLAY_BACKGROUND_AUDIO_1:
      currentMusic = BACKGROUND_AUDIO;  // Indicate bg music
      player.play(BACKGROUND_AUDIO);    // Play bg music
      isMusicPlaying = true;            // Indicate music is playing
      Serial.println("Playing bg song...");
      break;
    case WAIT_SIREN_TRIGGER:
      break;
    case PLAY_SIREN_AUDIO:
      currentMusic = SIREN_AUDIO;  // Indicate siren music
      player.play(SIREN_AUDIO);    // Play siren music
      isMusicPlaying = true;       // Indicate music is playing
      Serial.println("Playing siren song...");
      break;
    case WAIT_SIREN_AUDIO_FINISHED:
      break;
    case PLAY_BACKGROUND_AUDIO_2:
      currentMusic = BACKGROUND_AUDIO;  // Indicate bg music
      player.play(BACKGROUND_AUDIO);    // Play bg music
      isMusicPlaying = true;            // Indicate music is playing
      Serial.println("Playing bg song...");
      break;
    case WAIT_BACKGROUND_AUDIO_FINISHED:
      break;
    default:
      break;
  }

  if (player.available()) {
    printDetail(player.readType(), player.read());  //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void printDetail(uint8_t type, int value) {
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
      isMusicPlaying = false;
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
