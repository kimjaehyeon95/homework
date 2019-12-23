#include <SoftwareSerial.h>

// TX 3번 핀, RX 13번 핀으로 하여 bluetooth 설정
SoftwareSerial bluetooth(3, 13);

// pin_1, pin_2: 적외선 센서 연결 pin
// LED_1, LED_2: 센서에 감지 되었을 때 켜지는 LED
// LED_3: 출입이 1 이상이면 불이 들어오는 LED
int pin_1 = A0;
int pin_2 = A5;
int LED_1 = 4;
int LED_2 = 2;
int LED_3 = 6;

// 초음파 센서의 정보를 가져오는 pin 
int echo_1 = 7;
int echo_2 = 9;
int echo_3 = 11;
int trig_1 = 8;
int trig_2 = 10;
int trig_3 = 12;

// chk_1, chk_2: 출입을 check하기 위한 변수
// count: 출입 count를 세는 변수
int chk_1 = 0;
int chk_2 = 0;
int count = 0;

// millis를 통해 delay를 조절하기 위한 변수
unsigned long pre_time1 = 0;
unsigned long pre_time2 = 0;
unsigned long cur_time = 0;

// start_1, start_2: 각 loop의 처음 시작을 위한 변수
int start_1 = 0;
int start_2 = 0;

// dist: 초음파 센서로 측정할 거리
int dist = 15;

// stud_1, 2, 3: 학생의 수업 참여도를 보이는 변수
int stud_1 = 0;
int stud_2 = 0;
int stud_3 = 0;

// 초음파센서로 거리를 측정하는 함수
long readUltrasonicDistance(int echo, int trig) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(pin_1, INPUT);
  pinMode(pin_2, INPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  
  pinMode(echo_1, INPUT);
  pinMode(echo_2, INPUT);
  pinMode(echo_3, INPUT);
  pinMode(trig_1, OUTPUT);
  pinMode(trig_2, OUTPUT);
  pinMode(trig_3, OUTPUT);
  digitalWrite(trig_1, LOW);
  digitalWrite(trig_2, LOW);
  digitalWrite(trig_3, LOW);

  pre_time1 = millis();
  pre_time2 = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  int value1 = analogRead(pin_1);
  int value2 = analogRead(pin_2);
  cur_time = millis();  

  // 적외선 센서에 물체가 감지되지 않으면 LED가 꺼짐
  if(value1 > 400) {
    digitalWrite(LED_1, LOW);
  }
  if(value2 > 400) {
    digitalWrite(LED_2, LOW);
  }

  // loop() 함수 실행 후 1초에 한 번씩 조건문 내부 코드 실행
  if(start_1 == 0 | cur_time - pre_time1 >= 1000) {
    // 들어가는 쪽의 센서 감지 시 LED_1의 불이 켜짐
    // chk_2 == 0이면 chk_1을 1로 바꾸어 들어가는 쪽 센서에 물체가 감지 되었음을 인지
    if(chk_2 == 0 & value1 <= 400) {
      chk_1 = 1;
      digitalWrite(LED_1, HIGH);
    }

    // 나오는 쪽의 센서가 감지 된 상태에 들어가는 쪽 센서 물체 감지 시
    // 사람이 나오는 것이기에 count를 1 감소
    // count가 음수가 안되도록 조건 추가
    // count에 변화가 생겼기에 이를 Serial monitor에 표시
    // delay를 위해 pre_time1을 현재 시간인 cur_time으로 바꿈
    // 실행이 되면 start_1에 의해 if문 내의 진입을 막기 위해 값 변환
    else if(chk_2 == 1 & value1 <= 400) {
      chk_2 = 0;
      count--;
      if (count < 0) {
        count = 0;
      }
      Serial.print(" Count: ");
      Serial.print(count);
      Serial.println("");
      pre_time1 = cur_time;
      start_1 = 1;
      digitalWrite(LED_1, HIGH);
    }
    
    // 나오는 쪽의 센서
    // 들어가는 센서가 먼저 감지되고 나오는 센서가 감지되는 경우 사람이 들어오는 것이기에 count 1 증가
    if(chk_1 == 0 & value2 <= 400) {
      chk_2 = 1;
      digitalWrite(LED_2, HIGH);
    }
    else if(chk_1 == 1 & value2 <= 400) {
      chk_1 = 0;
      count++;
      pre_time1 = cur_time;
      start_1 = 1;
      digitalWrite(LED_2, HIGH);
      Serial.print(" Count: ");
      Serial.print(count);
      Serial.println("");
    }
  }

  // 사람이 1명 이상 들어왔을 경우 LED에 불이 켜지게 함
  if (count > 0 ) {
    digitalWrite(LED_3, HIGH);
  }
  else {
    digitalWrite(LED_3, LOW);
  }

  // loop() 함수 실행 후 5초에 한 번씩 조건문 내부 코드 실행
  if (start_2 == 0 | cur_time - pre_time2 >= 5000) {
    // 각 초음파 센서의 측정 거리를 저장
    int cm_1 = 0.01723 * readUltrasonicDistance(echo_1, trig_1);
    int cm_2 = 0.01723 * readUltrasonicDistance(echo_2, trig_2);
    int cm_3 = 0.01723 * readUltrasonicDistance(echo_3, trig_3);

    // 측정 거리를 Serial monitor에 표현
    Serial.print(cm_1);
    Serial.print("cm  ");    
    Serial.print(cm_2);
    Serial.print("cm  ");    
    Serial.print(cm_3);
    Serial.println("cm");

    // 특정 거리 이상인 경우 학생이 자리에 없다고 판단
    // 학생이 수업에 많이 참여했으면 0에 가까움
    if (cm_1 > dist) {
      stud_1++;
    }
    if (cm_2 > dist) {
      stud_2++;
    }
    if (cm_3 > dist) {
      stud_3++;
    }

    // 수업 참여도를 Serial monitor에 표현
    Serial.print("Student 1: ");
    Serial.print(stud_1);
    Serial.print("  Student 2: ");
    Serial.print(stud_2);
    Serial.print("  Student 3: ");
    Serial.println(stud_3);
    pre_time2 = cur_time;
    start_2 = 1;

    // bluetooth 설정
    char val = bluetooth.read();

    if (bluetooth.available()) {
      Serial.write(bluetooth.read());
    }

    // bluetooth로 연결된 기기에서 e를 입력하면 출입 수와 학생들의 참여도를 해당 기기에 출력
    if (val == 'e') {
      char temp[33];
      bluetooth.write("출입 수: ");
      itoa(count, temp, 10);
      bluetooth.write(temp);
      bluetooth.write("\n학생 1의 참여도: ");
      itoa(stud_1, temp, 10);
      bluetooth.write(temp);
      bluetooth.write(", 학생 2의 참여도: ");
      itoa(stud_2, temp, 10);
      bluetooth.write(temp);
      bluetooth.write(", 학생 3의 참여도: ");
      itoa(stud_3, temp, 10);
      bluetooth.write(temp);
      bluetooth.write("\n");
    }
  }
}
