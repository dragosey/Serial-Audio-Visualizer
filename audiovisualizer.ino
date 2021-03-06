#include <Adafruit_NeoPixel.h>

#define PIN 11
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

const int ledPin = 13;

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};
int newFlashInterval = 0;

unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;
int soundValue = 0;
int numberOfLeds = 0;

//=============

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  strip.begin();
  strip.setBrightness(50); //adjust brightness here
  strip.show(); // Initialize all pixels to 'off'

  for(int i = 0; i < 16; i++) {
      strip.setPixelColor(i,10,0,0);
      strip.show();
  }
  delay(2000); //init
  for(int i = 0; i < 16; i++) {
      strip.setPixelColor(i,0,0,0);
      strip.show();
  }
  delay(2000);
  // tell the PC we are ready
  Serial.println("<Arduino is ready>");
}

//=============

void loop() {
  curMillis = millis();
  getDataFromPC();
  replyToPC();
  flashLEDs();
}

//=============

void getDataFromPC() {

    // receive data from PC and save it into inputBuffer
    
  if(Serial.available() > 0) {

    char x = Serial.read();

      // the order of these IF clauses is significant
      
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  }
}

//=============
 
void parseData() {
    char * strtokIndx; // this is used by strtok() as an index
//  
//  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
//  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
//  
//  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
//  newFlashInterval = atoi(strtokIndx);     // convert this part to an integer
//  
//  strtokIndx = strtok(NULL, ","); 
//  servoFraction = atof(strtokIndx);     // convert this part to a float

    strtokIndx = strtok(inputBuffer,">");
    strcpy(messageFromPC, strtokIndx);
    soundValue = atoi(strtokIndx); //Value to be used
   
}

//=============

void replyToPC() {

  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<Msg ");
    Serial.print(messageFromPC);
    Serial.print(" value ");
    Serial.print(soundValue);
    Serial.print(" Time ");
    Serial.print(curMillis >> 9); // divide by 512 is approx = half-seconds
    Serial.println(">");
  }
}

//=============

void flashLEDs() {
   float red = 0;
   float green = 0;
   float blue = 0;

   for(int i = 0; i < 16; i++) { // "black" color, resets leds
        strip.setPixelColor(i,0,0,0);
        strip.show();
   }

   if(soundValue < 100) {
    red = 0;
    green = 1;
    blue = 0;
   }
   else if(soundValue >= 100 && soundValue <= 150) {
    red = 1;
    green = 1;
    blue = 0;
   }
   else if(soundValue > 150 && soundValue <= 230) {
    red = 1.1;
    green = 0.2;
    blue = 0;
   }
   else if(soundValue > 230) {
    red = 1;
    green = 0;
    blue = 0;
   }
   
   numberOfLeds = (soundValue*16)/255;
   for(int i = 0; i < numberOfLeds; i++) {
      strip.setPixelColor(i,red*soundValue,green*soundValue,blue*soundValue);
      strip.show();
   }

}
