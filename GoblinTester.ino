const int phoneSensorPin=2;
const int breakButtonPin=3;
const int greenLedPin=6;
const int redLedPin=5;
const int buzzerPin=9;
int goblinPoints=0;
int pickupCount=0;
bool phoneWasPresent=false;
bool breakWasPressed=false;
unsigned long lastPickupTime=0;
unsigned long previousPickupTime=0;
unsigned long focusStartTime=0;
unsigned long lastButtonPress=0;
const unsigned long focusInterval=3600000;
const unsigned long spamPickupLimit=5000;
const unsigned long debounceDelay=500;

void beep(int onTime,int offTime){
  digitalWrite(buzzerPin,HIGH);
  delay(onTime);
  digitalWrite(buzzerPin,LOW);
  delay(offTime);
}
void sadTrombone(){
  beep(200,100);
  beep(200,100);
  beep(600,0);
}
void surprise(){
  beep(80,40);
  beep(700,0);
}
void angryChicken(){
  for(int i=0;i<10;i++)beep(30,30);
}
void alarmSiren(){
  for(int i=0;i<3;i++){
    for(int j=0;j<4;j++)beep(60,40);
    delay(200);
  }
}
void victoryJingle(){
  beep(150,80);
  beep(150,80);
  beep(500,0);
}
void flashLed(int pin,int timeOn){
  digitalWrite(pin,HIGH);
  delay(timeOn);
  digitalWrite(pin,LOW);
}

void printGoblinLevel(){
  Serial.print("Goblin points: ");
  Serial.println(goblinPoints);
  if(goblinPoints<=2)Serial.println("Human");
  else if(goblinPoints<=5)Serial.println("Slight Goblin");
  else if(goblinPoints<=9)Serial.println("Full Goblin");
  else Serial.println("DOOMSCROLL BEAST");
}
void handlePickup(unsigned long currentTime){
  Serial.println("Phone picked up");
  pickupCount++;
  goblinPoints++;
  bool pickedUpTooSoon=previousPickupTime>0&&currentTime-previousPickupTime<spamPickupLimit;
  if(pickedUpTooSoon){
    goblinPoints+=2;
    Serial.println("Spam pickup");
    alarmSiren();
    flashLed(redLedPin,300);
  }
  else if(pickupCount>=10){
    Serial.println("Doomscroll beast");
    angryChicken();
    flashLed(redLedPin,300);
  }
  else if(pickupCount==7){
    Serial.println("This is getting sad");
    sadTrombone();
    flashLed(redLedPin,300);
  }
  else if(pickupCount==5){
    Serial.println("Suspicious");
    surprise();
  }
  else sadTrombone();
  previousPickupTime=currentTime;
  lastPickupTime=currentTime;
  printGoblinLevel();
}
void handleReturn(){
  Serial.println("Phone returned");
  victoryJingle();
  flashLed(greenLedPin,500);
}
void handleBreak(unsigned long currentTime){
  if(currentTime-lastButtonPress<debounceDelay)return;
  Serial.println("Break started");
  goblinPoints-=3;
  if(goblinPoints<0)goblinPoints=0;
  pickupCount=0;
  previousPickupTime=0;
  lastButtonPress=currentTime;
  victoryJingle();
  flashLed(greenLedPin,500);
  printGoblinLevel();
}
void handleFocusHour(unsigned long currentTime){
  Serial.println("Focus hour completed");
  goblinPoints-=2;
  if(goblinPoints<0)goblinPoints=0;
  focusStartTime=currentTime;
  victoryJingle();
  flashLed(greenLedPin,500);
  printGoblinLevel();
}

void setup(){
  pinMode(phoneSensorPin,INPUT_PULLUP);
  pinMode(breakButtonPin,INPUT_PULLUP);
  pinMode(greenLedPin,OUTPUT);
  pinMode(redLedPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  digitalWrite(greenLedPin,LOW);
  digitalWrite(redLedPin,LOW);
  Serial.begin(9600);
  focusStartTime=millis();
  phoneWasPresent=digitalRead(phoneSensorPin);
  Serial.println("Goblin Phone Jail started");
  victoryJingle();
}
void loop(){
  bool phonePresent=digitalRead(phoneSensorPin);
  bool breakPressed=digitalRead(breakButtonPin)==LOW;
  unsigned long currentTime=millis();
  if(phoneWasPresent&&!phonePresent)handlePickup(currentTime);
  if(!phoneWasPresent&&phonePresent)handleReturn();
  phoneWasPresent=phonePresent;
  if(breakPressed&&!breakWasPressed)handleBreak(currentTime);
  breakWasPressed=breakPressed;
  if(currentTime-focusStartTime>=focusInterval)handleFocusHour(currentTime);
  if(goblinPoints>=10){
    flashLed(redLedPin,200);
    delay(200);
  }
  delay(50);
}
