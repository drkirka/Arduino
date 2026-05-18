const int obstaclePin=2;
const int greenLedPin=6;
const int redLedPin=5;
const int buzzerPin=9;
const int segA=3;
const int segB=4;
const int segC=7;
const int segD=8;
const int segE=10;
const int segF=11;
const int segG=12;
const bool obstacleActiveLow=true;
const bool commonAnode=false;
int goblinPoints=0;
int pickupCount=0;
bool phoneWasThere=false;

unsigned long previousPickupTime=0;
unsigned long focusStartTime=0;
unsigned long lastSensorChange=0;
bool lastRawPhone=false;
bool phoneThere=false;

const unsigned long focusInterval=3600000;
const unsigned long spamPickupLimit=5000;
const unsigned long sensorDelay=120;
void beep(int onTime,int offTime){
  digitalWrite(buzzerPin,HIGH);
  delay(onTime);
  digitalWrite(buzzerPin,LOW);
  delay(offTime);
}
void sadTrombone(){
  beep(200,100);beep(200,100);beep(600,0);
}
void surprise(){
  beep(80,40);beep(700,0);
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
  beep(150,80);beep(150,80);beep(500,0);
}
void flashLed(int pin,int timeOn){
  digitalWrite(pin,HIGH);
  delay(timeOn);
  digitalWrite(pin,LOW);
}

bool readPhoneSensor(){
  int v=digitalRead(obstaclePin);
  if(obstacleActiveLow)return v==LOW;
  return v==HIGH;
}
void segWrite(int pin,bool on){
  if(commonAnode)digitalWrite(pin,on?LOW:HIGH);
  else digitalWrite(pin,on?HIGH:LOW);
}
void showDigit(int n){
  if(n<0)n=0;
  if(n>9)n=9;
  bool nums[10][7]={
    {1,1,1,1,1,1,0},
    {0,1,1,0,0,0,0},
    {1,1,0,1,1,0,1},
    {1,1,1,1,0,0,1},
    {0,1,1,0,0,1,1},
    {1,0,1,1,0,1,1},
    {1,0,1,1,1,1,1},
    {1,1,1,0,0,0,0},
    {1,1,1,1,1,1,1},
    {1,1,1,1,0,1,1}
  };
  segWrite(segA,nums[n][0]);segWrite(segB,nums[n][1]);segWrite(segC,nums[n][2]);
  segWrite(segD,nums[n][3]);segWrite(segE,nums[n][4]);segWrite(segF,nums[n][5]);segWrite(segG,nums[n][6]);
}
void printLevel(){
  Serial.print("points ");Serial.println(goblinPoints);
  if(goblinPoints<=2)Serial.println("human");
  else if(goblinPoints<=5)Serial.println("little goblin");
  else if(goblinPoints<=9)Serial.println("full goblin");
  else Serial.println("doomscroll beast");
}
void setPoints(int n){
  goblinPoints=n;
  if(goblinPoints<0)goblinPoints=0;
  showDigit(goblinPoints);
}
void phoneTaken(unsigned long t){
  Serial.println("phone taken");
  pickupCount++;
  setPoints(goblinPoints+1);

  bool tooFast=previousPickupTime>0&&t-previousPickupTime<spamPickupLimit;
  if(tooFast){
    setPoints(goblinPoints+2);
    Serial.println("again too fast");
    alarmSiren();flashLed(redLedPin,300);
  }
  else if(pickupCount>=10){
    Serial.println("too many times");
    angryChicken();flashLed(redLedPin,300);
  }
  else if(pickupCount==7){
    sadTrombone();flashLed(redLedPin,300);
  }
  else if(pickupCount==5)surprise();
  else sadTrombone();

  previousPickupTime=t;
  printLevel();
}
void phoneBack(){
  Serial.println("phone back");
  victoryJingle();flashLed(greenLedPin,400);
}
void focusHour(unsigned long t){
  Serial.println("focus hour");
  setPoints(goblinPoints-2);
  pickupCount=0;
  previousPickupTime=0;
  focusStartTime=t;
  victoryJingle();flashLed(greenLedPin,500);
  printLevel();
}
void setup(){
  pinMode(obstaclePin,INPUT_PULLUP);
  pinMode(greenLedPin,OUTPUT);pinMode(redLedPin,OUTPUT);pinMode(buzzerPin,OUTPUT);
  pinMode(segA,OUTPUT);pinMode(segB,OUTPUT);pinMode(segC,OUTPUT);pinMode(segD,OUTPUT);
  pinMode(segE,OUTPUT);pinMode(segF,OUTPUT);pinMode(segG,OUTPUT);

  Serial.begin(9600);
  focusStartTime=millis();
  phoneThere=readPhoneSensor();
  phoneWasThere=phoneThere;
  lastRawPhone=phoneThere;
  showDigit(0);
  victoryJingle();
}
void loop(){
  unsigned long now=millis();
  bool raw=readPhoneSensor();

  if(raw!=lastRawPhone){
    lastSensorChange=now;
    lastRawPhone=raw;
  }
  if(now-lastSensorChange>sensorDelay)phoneThere=raw;
  if(phoneWasThere&&!phoneThere)phoneTaken(now);
  if(!phoneWasThere&&phoneThere)phoneBack();
  phoneWasThere=phoneThere;

  if(now-focusStartTime>=focusInterval)focusHour(now);
  if(goblinPoints>=10){
    flashLed(redLedPin,150);
    delay(150);
  }
  delay(30);
}
