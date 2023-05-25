#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);//lcd 제어, 주소확인 
SoftwareSerial bt(2,3);//블루투스 통신  

unsigned long time;//과거 시간 저장 변수 
//int flag = 0;//과거 기준 시간보자 일정시간 지날 경우를 판단하는 변수 

float volt = 5.0;

int MotorF1 = 9;//앞바퀴 제어 
int MotorF2 = 10;
int MotorB1 = 5;//뒷바퀴 제어   
int MotorB2 = 6;

int trig = 8;//초음파 센서1
int echo = 7;//초음파 센서2
int irsen = A0;//IR센서 A0
int dis;//초음파 거리 저장 변수
float irdis;//IR센서 거리 저장 변수 

int buzz = 11;//부저 

void setup(){
  bt.begin(9600);

  time = millis();

  lcd.init();//lcd초기화
  lcd.backlight();//lcd백라이트 켜기
  
  pinMode(MotorF1, OUTPUT);
  pinMode(MotorF2, OUTPUT);
  pinMode(MotorB1, OUTPUT);
  pinMode(MotorB2, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(buzz, OUTPUT);
  pinMode(irsen, INPUT);
  Serial.begin(9600);
}

void IR(){//IR센서 거리 측정 함수(pow함수는 거듭제곱을 구하는 함수)
  irdis = volt*analogRead(A0) / 1023;
  irdis = 26.549 * pow(irdis, -1.2091);

  return irdis;
}

void Frontwheel(int a){//전륜 제어 함수 
  if(a == '4'){
    lcd.clear();
    lcd.print("Left");
    analogWrite(MotorF1, 0);
    analogWrite(MotorF2,255);//좌회전
    delay(500);
    a = 'd';  
  }
  else if(a == '3'){
    lcd.clear();
    lcd.print("Right");
    analogWrite(MotorF1, 255);
    analogWrite(MotorF2, 0);//우회전 
    delay(500);
    a = 'd'; 
  }
  else if(a == 'd'){
    lcd.clear();
    lcd.print("Turn stop");
    analogWrite(MotorF1, 0);
    analogWrite(MotorF2, 0);//멈춤    
  }
}

void Backwheel(int b){//후륜 제어 함수 
  if(b == 'a'){
    lcd.clear();
    lcd.print("Gear 1");
    analogWrite(MotorB1, 0);
    analogWrite(MotorB2, 255/4);//전진(25%)  
  }
  else if(b == 'b'){
    lcd.clear();
    lcd.print("Gear 2");
    analogWrite(MotorB1, 0);
    analogWrite(MotorB2, 255/2);//전진(50%)
  }
  else if(b == 'c'){
    lcd.clear();
    lcd.print("Gear 3");
    analogWrite(MotorB1, 0);
    analogWrite(MotorB2, 255*3/4);//전진(75%)  
  }
  else if(b == '1'){
    lcd.clear();
    lcd.print("Gear 4");
    analogWrite(MotorB1, 0);
    analogWrite(MotorB2, 255);//전진(100%)
  }
  else if(b == '2'){
    lcd.clear();
    lcd.print("Gear R");
    analogWrite(MotorB1, 255);
    analogWrite(MotorB2, 0);//후진   
  }
  else if(b == '0'){
    lcd.clear();
    lcd.print("STOP");
    analogWrite(MotorB1, 255);
    analogWrite(MotorB2, 0);
    delay(400);//0.4초간 후진  
    analogWrite(MotorB1, 0);
    analogWrite(MotorB2, 0);//정지 
  }
}

int Distmeasure(){ //초음파 센서 거리측정 함수 
  digitalWrite(trig, HIGH);
  delay(100);
  digitalWrite(trig, LOW);

  dis = pulseIn(echo, HIGH)*340/2/10000;

  return dis;
}

void alarmsound(int c){//부저 소리 함수  
  if(c>=100){
    noTone(buzz);
    delay(1000);//소리 끔 
  }
  else if(c<100 && c>=50)
  {
    tone(buzz,261);
    delay(500); noTone(buzz);
  }
  else if(c<50 && c>=30)
  {
    lcd.clear();
    tone(buzz,261);
    delay(100); noTone(buzz);
  }
  else if(c<30 && c>=15)
  {
    lcd.clear();
    lcd.print("Close");
    tone(buzz,261);
    delay(50); noTone(buzz);
  }
  else if(c<15 && c>=8)
  {
    lcd.clear();
    lcd.print("So Close!");
    tone(buzz,261);
    delay(10); noTone(buzz);
    
  }
  else if(c<8)
  {
    lcd.clear();
    lcd.print("Warning!");
    tone(buzz,261);
    delay(2); noTone(buzz);
  }
}

int safestop(int d){//안전 멈춤 
  if(d==20){
    analogWrite(MotorB1, 255);
    analogWrite(MotorB2, 0);
    delay(300);
    analogWrite(MotorB1, 0);
    analogWrite(MotorB2, 0);
  }
}

void loop(){
  unsigned long now = millis();
  int data = bt.read();
  IR();
  Frontwheel(data);
  Backwheel(data);
  safestop(irdis);

  Serial.println(irdis);
  Serial.println();
  /*alarmsound(irdis);

  /*int state = digitalRead(irs);
  
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  int dis = pulseIn(echo, HIGH)*340/2/10000;
  */
}
