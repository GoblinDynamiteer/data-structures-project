#include <Wire.h>
const int MPU_addr = 0x68;  // I2C address of the MPU-6050
int16_t AcZ;
uint16_t ticker = 0;
uint16_t diff = 0;
int time;

/*  Struct för hållande av slag */
typedef struct{
  int16_t styrka;
  int tid;
  int langd;
  bool typ;
}slagS;

enum{FOREHAND, BACKHAND};
#define MAXSLAG 100

/*  Struct array */
slagS slag[MAXSLAG];
byte antal = 0;
int ix = 0;

void readMPUData(){
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
    AcZ = Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
}

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  int countdown = 10;
  /*  Hitta diff-värde för kalibration   */
  Serial.print("Calibrating completed in ");
  for(int i = 0; i < 10; i++){
    readMPUData();
    diff = diff + AcZ;
    delay(1000);
    Serial.println(countdown--);
    Serial.println(AcZ);
  }
  diff = diff / 10;
}
void loop(){
  readMPUData();
  AcZ = ((AcZ - diff));
  if(AcZ < -3300){
    time = millis();
    Serial.println("Forehand");
    Serial.println(AcZ);
    slag[ix].styrka = fabs(AcZ);
    slag[ix].tid = (int)time;
    slag[ix].typ = FOREHAND;
    while(fabs(AcZ) > 400){
        readMPUData();
        AcZ = ((AcZ - diff));
    }
    time = millis() - time;
    Serial.println("tid för slag:");
    Serial.print(time);
    slag[ix].langd = (int)time;
    delay(10);
    ix++;
  }

  if(AcZ > 3300){
    time = millis();
    Serial.println("Backhand");
    Serial.println(AcZ);
    slag[ix].styrka = fabs(AcZ);
    slag[ix].tid = (int)time;
    slag[ix].typ = BACKHAND;
    while(fabs(AcZ) > 400){
        readMPUData();
        AcZ = ((AcZ - diff));
    }
    time = millis() - time;
    Serial.println("tid för slag:");
    Serial.print(time);
    slag[ix].langd = (int)time;
    delay(10);
    ix++;
  }
  Serial.print("----IX: "); Serial.println(ix);
    if(ix == MAXSLAG-1){
        Serial.println("DONE!");
        for(int i = 0; i < MAXSLAG; i++){
            Serial.print("Slag ");
            Serial.println(i+1);
            Serial.print("Typ: ");
            Serial.println(slag[i].typ);
            Serial.print("Styrka: ");
            Serial.println(slag[i].styrka);
            Serial.print("Tid: ");
            Serial.println(slag[i].tid);
            Serial.print("Langd: ");
            Serial.println(slag[i].langd);
            Serial.println("---------------------");
        }
    }
  delay(10);
}
