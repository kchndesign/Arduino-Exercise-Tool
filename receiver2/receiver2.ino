//*****************************
//this code is testing the reciever number 2
//kevin chen smart design 2021
//**********************

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7,8); //CNS, CE pins

//listening and transmisson addresses, these need to be swapped for the transmitter and reciever
const byte addresses[] [6] = {"00002", "00003"};


//predetermined radio messages
const int commandOne = 2;
const int replyOne = 22;
int button = A1; 
int led = 3;
bool activated = false; //boolean to store activated or not.


void setup() {
  // put your setup code here, to run once:
  pinMode(button, INPUT); //button pin for testing
  pinMode(led, OUTPUT); //led pin

  Serial.begin(9600);

  radio.begin(); //start setup for radio
  radio.openWritingPipe(addresses[0]); //address for transmitting
  radio.openReadingPipe(1, addresses[1]);//swapped for reciever
  radio.setPALevel(RF24_PA_MIN); //minimum power
  radio.setDataRate(2);
  radio.setChannel(120);
  
  
}
void loop() {

//if this is not activated then keep listening.
if (activated == false){ 
  
  radio.startListening();
  digitalWrite(led, LOW);

  //if recieve data, check and activate
  if (radio.available()){
    Serial.println("radio available triggered");
    digitalWrite(led, HIGH);
    //store data in temp
      int dataR = 0;
      radio.read(&dataR, sizeof(dataR));
      //if data correct, activate and turn on LED
      if (dataR == commandOne){
        activated=true;
        digitalWrite(led, HIGH);
     
    }
  }
}

//if tthis node is activated:
if (activated == true){
  
  digitalWrite(led, HIGH);
  Serial.println(analogRead(button));

  //wait for button press
  if (analogRead(button)>300){
    //stop listening
    radio.stopListening();
    //turn off led
    digitalWrite(led, LOW);
    //send reply
    bool report = radio.write(&replyOne, sizeof(replyOne));
    while(!report){
      delay(150);
      Serial.println("what the heck is this");
      report = radio.write(&replyOne, sizeof(replyOne));
    }
    //activated off
    activated = false;
  }
}

}
