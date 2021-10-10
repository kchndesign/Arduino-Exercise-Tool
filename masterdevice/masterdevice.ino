//Kevin Chen Smart Design 2021 
//Ultrasonic Distance Sensor
//Sports Training tool.

//Sensor 1 values
const int trigPinOne = 9;
const int echoPinOne = 10;
const int ledPinOne = 12;
const int boardPinOne = 5;

//Sensor 2 Values
const int trigPinTwo = 7;
const int echoPinTwo = 8;
const int ledPinTwo = 13;
const int boardPinTwo = 4;

//other variabls
long duration; //initial return in microseconds
int distance; //calculated distance
int trigDistance = 80; //threshold distance to trigger sensor.
int sensNum; //var to store currently selected sensor.

void setup() {

  //set pinmodes for Sensor 1
  pinMode(trigPinOne, OUTPUT); 
  pinMode(echoPinOne, INPUT);
  pinMode(ledPinOne, OUTPUT);
  pinMode(boardPinOne, OUTPUT);

  //set pinmodes for Sensor 2
  pinMode(trigPinTwo, OUTPUT); 
  pinMode(echoPinTwo, INPUT);
  pinMode(ledPinTwo, OUTPUT);
  pinMode(boardPinTwo, OUTPUT);

  Serial.begin(9600); //start serial communication

  //select random sensor out of two
  sensNum = round(random(0,2));
  Serial.print(sensNum); 
  
}

void loop() {

//find out which sensor is currently selected
if (sensNum < 0.5){

  //turn on LED or reset to HIGH 
  digitalWrite(ledPinOne, HIGH);
  digitalWrite(boardPinOne, HIGH);
   
  //reset trigPin
  digitalWrite(trigPinOne, LOW);
  delayMicroseconds(2);


  //10 microseconds for the outgoing ultrasound
  digitalWrite(trigPinOne, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinOne, LOW);

  //measure the sound wave travel time in microseconds
  duration = pulseIn(echoPinOne, HIGH);

  //calculating the distance
  distance = duration*0.034/2;

  if (distance < 80){
    //if player detected, turn off both LED
    digitalWrite(ledPinOne, LOW);
    digitalWrite(boardPinOne, LOW);
    //wait for a bit and then randomly select a new sensor
    delay(1500);
    sensNum = (round(random(0,2)));
  }

  else{
    digitalWrite(ledPinOne, HIGH);
    digitalWrite(boardPinOne, HIGH);
  }

  //print the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);



}

else{
  //turn on LED or reset to HIGH 
  digitalWrite(ledPinTwo, HIGH);
  digitalWrite(boardPinTwo, HIGH);
   
  //reset trigPin
  digitalWrite(trigPinTwo, LOW);
  delayMicroseconds(2);


  //10 microseconds for the outgoing ultrasound
  digitalWrite(trigPinTwo, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinTwo, LOW);

  //measure the sound wave travel time in microseconds
  duration = pulseIn(echoPinTwo, HIGH);

  //calculating the distance
  distance = duration*0.034/2;

  if (distance < 80){
    //if player detected, turn off both LED
    digitalWrite(ledPinTwo, LOW);
    digitalWrite(boardPinTwo, LOW);
    //wait for a bit and then randomly select a new sensor
    delay(1500);  
    sensNum = (round(random(0,2)));
  }

  else{
    digitalWrite(ledPinTwo, HIGH);
    digitalWrite(boardPinTwo, HIGH);
  }

  //print the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
}
  

}
