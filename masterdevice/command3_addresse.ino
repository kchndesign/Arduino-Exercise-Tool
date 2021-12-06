//*****************************
//this code is for the controller
//kevin chen smart design 2021
//**********************

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define PIN 6
#define NUMPIXELS 8

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

RF24 radio(7, 8); //CNS, CE pins

//listening and transmisson addresses, these need to be swapped for the transmitter and reciever
const byte addresses[] [6] = {"00001", "00002", "00003"};

//predetermined radio messages
const int commandOne = 1;
const int replyOne = 11;
const int commandTwo = 2;
const int replyTwo = 22;

//setup IO
int button = 3;
int button2 = 1;
int led = 4;
int led2 = 5;
int ledbar = 6;
int poten = A1;
int modePin = 2;
int activated = 0; //integer to store the currently activated node of none are activated then its zero


void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  pinMode(button, INPUT_PULLUP); //button pin for testing PULLUP MEANS WRIE FROM INPUT TO GROUND
  pinMode(button2, INPUT_PULLUP);//button pin for node 2 PULLUP MEANS WRIE FROM INPUT TO GROUND
  pinMode(led, OUTPUT); //led pin
  pinMode(led2, OUTPUT);//led pin for node 2
  pinMode(ledbar, OUTPUT); //8 led bar neopixel thing
  pinMode(poten, INPUT); //wire ends to gnd and +5v and middle to the A1
  pinMode(modePin, INPUT_PULLUP);//mode select on the hook. PULLUP MEANS WirE FROM INPUT TO GROUND

  //radio setup
  radio.begin(); //start setup for radio
  radio.openWritingPipe(addresses[0]); //address for transmitting
  radio.openReadingPipe(1, addresses[1]);
  radio.openReadingPipe(2, addresses[2]);
  radio.setPALevel(RF24_PA_MIN); //minimum power
  uint8_t channel = radio.getChannel();
  Serial.println(channel);
  radio.setDataRate(2);
  radio.setChannel(120);

  //neopixels setup for the led bar
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  pixels.begin();


}
void loop() {

  //check the mode using the modePin input
  //DEFAULT RANDOM MODE
  if (digitalRead(modePin) == HIGH) {

    //PACE DELAY SETTINGS
    int paceInt = 1300; //integer in ms for time between play completing shot and new one slected. default is 1300 ms
    int paceCount = 0; //counter to keep track of shots played.

    //check if the player has selected manual pace
    if (analogRead(poten) <= 10) {
      //if automatic pace selected.

      //basic calculation based on number of shots played.
      //for every shot played, decrease 5 milliseconds
      paceInt = paceInt - (paceCount * 5);
      //bottom out pace at 100 milliseconds
      if (paceInt < 100) {
        paceInt = 100;
      }

      //update neopixels

      //map pace value to 8 pixels.

      int pixNum = map(paceInt, 100, 1300, 1, 8);

      //display number of pixels contained in integer
      //pixels are addressed individually so we must iterate through them
      pixels.clear();
      for (int i = 0; i < pixNum; i++) {
        //blue colour for auto mode.
        pixels.setPixelColor(i, pixels.Color(0, 0, 150));
      }
      pixels.show();
    }

    if (analogRead(poten) > 10) {
      //manual pace selected

      //map potentiometer values between 1300 and 650 for reasonable control
      paceInt = map(analogRead(poten), 15, 1000, 650, 1300);

      //map the pixels values
      int pixNum = map(paceInt, 650, 1300, 1, 8);

      //display number of pixels contained in integer
      pixels.clear();
      for (int i = 0; i < pixNum; i++) {
        //green color for manual mode.
        pixels.setPixelColor(i, pixels.Color(0, 150, 0));
      }
      pixels.show();
    }

    //check if there are any nodes activated at the moment.
    if (activated == 0) {
      //start transmitting mode.
      radio.stopListening();
      //pick a random node out of two.
      int selected = round(random(0, 2));
      Serial.println(selected);

      //if node 1 is selected
      if (selected < 0.5) {

        //start transmisson
        //select correct pipe
        radio.openWritingPipe(addresses[0]);
        //bool to store successful message or not
        bool report = radio.write(&commandOne, sizeof(commandOne)); //also transmits the data at the same time

        //if not recieved
        while (!report) {
          delay(200);
          //try again
          report = radio.write(&commandOne, sizeof(commandOne));
          //turn off led
          digitalWrite(led, LOW);
          Serial.println("transmission failed or timed out for node 1");

          //pixels red
          pixels.clear();
          for (int i = 0; i < 8; i++) {
            //green color for manual mode.
            pixels.setPixelColor(i, pixels.Color(150, 0, 0));
          }
          pixels.show();
        }

        //turn on LED
        digitalWrite(led, HIGH);

        //activated
        activated = 1;

        Serial.println("node one selected");
      }

      //if node 2 is selected
      if (selected > 0.5) {
        Serial.println("attemptin to transmit to node 2");
        //start transmisson
        //select correct pipe 
        radio.openWritingPipe(addresses[2]);
        //bool to store successful message or not
        bool report = radio.write(&commandTwo, sizeof(commandTwo)); //also transmits the data at the same time
        Serial.println(report);
        //if not recieved
        while (!report) {
          Serial.println("transmisson failed or timed out for node 2");
          delay(200);
          //try again
          report = radio.write(&commandTwo, sizeof(commandTwo));
          //turn off led
          digitalWrite(led2, LOW);

          //pixels red
          pixels.clear();
          for (int i = 0; i < 8; i++) {
            //green color for manual mode.
            pixels.setPixelColor(i, pixels.Color(150, 0, 0));
          }
          pixels.show();
        }

        //turn on LED
        digitalWrite(led2, HIGH);

        //activated
        activated = 2;

        Serial.println("node two selected");

      }
    }

    //if node one is currently active
    if (activated == 1) {
      radio.startListening();
      
      //led on for sanity
      digitalWrite(led, HIGH);

      //if there is a message
      if (radio.available()) {
        Serial.println("message received while waiting for node one");
        while (radio.available()) {
          //store data in temp var
          int dataR = 0;
          radio.read(&dataR, sizeof(dataR));
          Serial.println("data read");
          //if the data is a reply from the right one, then turn off the led, change the activated variable.
          if (dataR == 11) {
            Serial.println("data verified");
            digitalWrite(led, LOW);
            activated = 0;
            //add increment to delay counter
            paceCount = paceCount + 1;
            //delay before returning to loop
            delay(paceInt);
          }
        }
      }
    }

    // if node two is currently active
    if (activated == 2) {
      radio.startListening();
      //led on for sanity
      digitalWrite(led2, HIGH);

      //if there is a message
      if (radio.available()) {
        Serial.println("message recieved while waiting for node 2");
        while (radio.available()) {
          //store data in temp var
          int dataR = 0;
          radio.read(&dataR, sizeof(dataR));
          //if the data is a reply from the right one, then turn off the led, change the activated variable.
          if (dataR == 22) {
            digitalWrite(led2, LOW);
            activated = 0;
            //add increment to delay counter
            paceCount = paceCount + 1;
            //delay before returning to loop
            delay(paceInt);
          }
        }
      }
    }
  }

  //CONTROLLER MODE
  if (digitalRead(modePin) == LOW) {
    //turn off pixel leds
    pixels.clear();

    //if there are no nodes activated
    if (activated == 0) {
      //stop listening
      radio.stopListening();
      //wait for button press
      //BUTTON ONE
      if (digitalRead(button) == HIGH) {
        //start transmitting
        bool report = radio.write(&commandOne, sizeof(commandOne));
        //check if successfully transmitted
        if (!report) {
          //turn led red
          pixels.clear();
          for (int i = 0; i < 8; i++) {
            pixels.setPixelColor(i, pixels.Color(150, 0, 0));
          }
          pixels.show();
          //set activated = 0 to return to loop()
          activated = 0;
        } else {
          //turn on corresponding led.
          digitalWrite(led, HIGH);
          activated = 1;
        }
      }
      //BUTTON TWO
      if (digitalRead(button) == HIGH) {
        //start transmitting
        bool report = radio.write(&commandTwo, sizeof(commandTwo));
        //check if successfully transmitted
        if (!report) {
          //turn led red
          pixels.clear();
          for (int i = 0; i < 8; i++) {
            pixels.setPixelColor(i, pixels.Color(150, 0, 0));
          }
          pixels.show();
          //set activated = 0 to return to loop()
          activated = 0;
        } 
        else {
          //turn on corresponding led.
          digitalWrite(led2, HIGH);
          activated = 2;
        }
      }
    }
    if (activated == 1) {
      //start listening
      radio.startListening();
      //led for sanity
      digitalWrite(led, HIGH);
      //wait for reply
      if (radio.available()) {
        while (radio.available()) {
          //store data in temp var
          int dataR = 0;
          radio.read(&dataR, sizeof(dataR));
          //if the data is a reply from the right one, then turn off the led, change the activated variable.
          if (dataR == 11) {
            digitalWrite(led, LOW);
            activated = 0;
          }
        }
      }
    }
    if (activated == 2) {
      //start listening
      radio.startListening();
      Serial.println("listening on node two");
      //led for sanity
      digitalWrite(led2, HIGH);
      //wait for reply
      if (radio.available()) {
        while (radio.available()) {
          //store data in temp var
          int dataR = 0;
          radio.read(&dataR, sizeof(dataR));
          //if the data is a reply from the right one, then turn off the led, change the activated variable.
          if (dataR == 22) {
            digitalWrite(led2, LOW);
            activated = 0;
            
          }
        }
      }
    }
  }
}
