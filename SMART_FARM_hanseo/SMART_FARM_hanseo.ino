#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // LCD를 사용하기 위한 라이브러리 추가
#include <DHT.h> // 온습도센서를 사용하기 위한 라이브러리 추가
#define DHTPIN 2 // 온습도센서 데이터단자 연결 핀번호
#define DHTTYPE DHT11 // DHT11 센서일 경우 DHT11 , DHT22 센서일 경우 DHT22
#define A_IA 9 // 모터드라이버 A_1A 단자 연결 핀번호(워터모터)
#define A_IB 8 // 모터드라이버 A_1B 단자 연결 핀번호
#define A_I1 10 // 팬 모터드라이버 A_1A 단자 연결 핀번호
#define A_I2 11 // 팬 모터드라이버 A_2B 단자 연결 핀번호
#define B_IA 6 // 모터드라이버 B_1A 단자 연결 핀번호 (미니모터)
#define B_IB 5 // 모터드라이버 B_1B 단자 연결 핀번호
#define SOIL_HUMI A0

LiquidCrystal_I2C lcd(0x27, 16, 4); // LCD I2C 통신 설정(주소, 글자수, 줄수) // 보_A4, 초_A5
DHT dht(DHTPIN, DHTTYPE); // 온습도센서 사용 설정

int cds = A1; // 조도센서에 사용할 핀번호
int relay = 3; // relay LED에 사용할 핀번호
int soil, psoil;  // 수분센서 값을 사용하기 위한 변수 선언
int val, ledval, pledval; // 조도센서 값을 사용하기 위한 변수 선언
int pgas;
int hval, tval;

void setup() { 
 Serial.begin(9600); // 시리얼 모니터 사용
 dht.begin(); // 온습도센서 작동
 
 pinMode(A_IA, OUTPUT); // 워터 모터 출력모드
 pinMode(A_IB, OUTPUT);  
 analogWrite(A_IA, LOW); // 워터 모터 초기값 OFF 상태
 analogWrite(A_IB, LOW);
 
 pinMode(A_I1, OUTPUT); // 팬 모터 출력모드
 pinMode(A_I2, OUTPUT);
 pinMode(B_IA, OUTPUT);
 pinMode(B_IB, OUTPUT);
 analogWrite(A_I1, LOW); // 팬 모터 초기값 OFF 상태
 analogWrite(A_I2, LOW);
 analogWrite(B_IA, LOW);
 analogWrite(B_IB, LOW);
 pinMode(relay, OUTPUT); // relay LED 출력 모드
 digitalWrite(relay, LOW); // LED 초기값 OFF 상태
}

void loop() {  
 float h = dht.readHumidity(); // 습도 값을 읽어 h에 입력, 그 값은 실수임 (온습도센서는 소수점 2자리까지 나타내도록 제작되어 있음)
 float t = dht.readTemperature(); // 온도 값을 읽어 t에 입력

 if (isnan(h) || isnan(t)){ // 온습도센서 가 읽은 값이 숫자가 아니면
  Serial.println("DHT센서 값 읽기 실패!"); // 문자 출력
  return; //프로그램 종료
  }
 soil = analogRead(SOIL_HUMI); // A0에서 읽은 값을 soil 변수에 저장
 psoil = map(soil, 1023, 0, 0, 100); // map함수를 사용하여 soil값을 1~100으로 변환한 값을 psoil에 저장
 int gas = analogRead(A2);  // A2에서 읽은 값을 gas 변수에 저장
 pgas = map(gas,0,1023,0,100);
 val = analogRead(cds); // A1에서 읽은 값을 val 변수에 저장
 ledval = map(val,0, 1023, 250, 0); // map함수를 사용하여 val값을 1~250으로 변환한 값을 ledval에 저장
 pledval = ledval*0.4; // 조도센서값을 0~100으로 표시하기 위한 설정
 //int led = 100 - pledval;
 lcd.init(); // LCD 초기화 
 lcd.clear(); // 이전에 출력한 값 지우기 
 lcd.backlight(); // 배경화면 빛이 들어오도록 설정 
 lcd.display(); // 내용을 표시
 lcd.setCursor(0,0);
 lcd.print("Soil : ");
 lcd.print(psoil);
 lcd.setCursor(11,0);
 lcd.print("Gas : ");
 lcd.print(pgas);  
 lcd.setCursor(0,1);
 lcd.print("Light: ");
 lcd.print(pledval);
 lcd.setCursor(11,1);
 lcd.print("Temp: ");
 lcd.print(t,0);  
 lcd.setCursor(0,2);
 lcd.print("Humid: ");
 lcd.print(h,0);
 lcd.setCursor(1,3);
 lcd.print("_Hanseo_Auto_Farm_");
 
 Serial.print(String(psoil) + "/");
 Serial.print(String(pgas) + "/");
 Serial.print(String(pledval) + "/");
 Serial.print(String(t,0) + "/");
 Serial.println(String(h,0));
 delay(900);
  
if(psoil < 30) { // 토양수분값이 30미만이면
 digitalWrite(A_IA, HIGH); // 워터모터를 돌려라
 digitalWrite(A_IB, LOW);
 analogWrite(A_IA, 255);  // 값을 변화시키면서 호스에서 나오는 물의 양을 적정하게 설정
 }
 else{  // 그 외 토양수분값이 측정되면 워터모터를 끄라
 digitalWrite(A_IA, LOW);
 digitalWrite(A_IB, LOW);
} 
if(t >= 35 || h >= 50 || pgas >= 70 ) { // 팬 동작 ON 조건1개(가스)
 digitalWrite(A_I1, HIGH);  // 1번 팬을 돌려라
 digitalWrite(A_I2, LOW);
 digitalWrite(B_IA, HIGH); // 2번 팬을 돌려라
 digitalWrite(B_IB, LOW);
 analogWrite(A_I1, 255); // 값을 변확시키면서 1번 팬의 세기를 설정
 analogWrite(B_IA, 255);  // 값을 변화시키면서 2번 팬의 세기를 설정
}
else{ // 그 외 온습도 측정값이면 미니모터를 끄라
 digitalWrite(A_I1, LOW);
 digitalWrite(A_I2, LOW);
 digitalWrite(B_IA, LOW);
 digitalWrite(B_IB, LOW);
}
 if (pledval <40) { // 조도센서값이 60미만이면
  digitalWrite(relay, 1);  // LED는 조도센서 값의 밝기로 켜라 
 } 
  else{  // 그 외 조도센서값이면 LED를 끄라
  digitalWrite(relay, 0);    
 }
}
