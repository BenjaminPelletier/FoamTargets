#include <Wire.h>

#ifdef ESP32
  const int PIN_MPU_A0[] = {14, 27, 26, 25, 33};
#else
  const int PIN_MPU_A0[] = {16, 0, 12, 13, 15}; // D0=GPIO16, D3=GPIO0, D6=GPIO12, D7=GPIO13, D8=GPIO15
#endif
const int MPU_COUNT = sizeof(PIN_MPU_A0) / sizeof(int);
const int ACCEL_OFF = LOW;
const int ACCEL_ON = HIGH;

const uint8_t MPU6050_ADDRESS = 0x69; // Device address when ADO = 1

const uint8_t SMPLRT_DIV = 0x19;
const uint8_t CONFIG = 0x1A;
const uint8_t GYRO_CONFIG = 0x1B;
const uint8_t ACCEL_CONFIG = 0x1C;
const uint8_t ACCEL_XOUT_H = 0x3B;
const uint8_t SIGNAL_PATH_RESET = 0x68;
const uint8_t PWR_MGMT_1 = 0x6B;
const uint8_t WHO_AM_I = 0x75; // Should return 0x68

void writeRegister(uint8_t slave_register, uint8_t value) {
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(slave_register);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t readRegister(uint8_t slave_register) {
  const uint8_t ONE_BYTE = 1;
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(slave_register);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDRESS, ONE_BYTE);
  return Wire.read();
}

void debugRegister(String register_name, uint8_t register_value) {
  Serial.print("0b");
  uint8_t v = register_value;
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(v >= 0x80 ? '1' : '0');
    v <<= 1;
  }
  Serial.print(" 0x");
  v = register_value >> 4;
  Serial.print((char)((v <= 9 ? '0' : '7') + v));
  v = register_value & 0x0F;
  Serial.print((char)((v <= 9 ? '0' : '7') + v));
  Serial.print(' ');
  Serial.print(register_value);
  Serial.print(' ');
  Serial.print(register_name);
  Serial.println();
}

void setup() {
  delay(1000);
  Serial.begin(57600);
  Serial.println("=====================================");
  
  for (int m = 0; m < MPU_COUNT; m++) {
    pinMode(PIN_MPU_A0[m], OUTPUT);
    digitalWrite(PIN_MPU_A0[m], ACCEL_OFF);
  }

  #ifdef ESP8266
    Wire.begin(4, 5); // SDA=D2=GPIO4, SCL=D1=GPIO5
  #else
    Wire.begin();
  #endif

  for (int m = 0; m < MPU_COUNT; m++) {
    digitalWrite(PIN_MPU_A0[m], ACCEL_ON);
    writeRegister(SMPLRT_DIV, 0x00);
    writeRegister(CONFIG, 0x00);
    writeRegister(GYRO_CONFIG, 0x08);
    writeRegister(ACCEL_CONFIG, 0x00);
    writeRegister(PWR_MGMT_1, 0x01);
    digitalWrite(PIN_MPU_A0[m], ACCEL_OFF);
  }

  for (int m = 0; m < MPU_COUNT; m++) {
    digitalWrite(PIN_MPU_A0[m], ACCEL_ON);
    Serial.print("Accelerometer ");
    Serial.println(m);
    debugRegister("WHO_AMI_I", readRegister(WHO_AM_I));
    debugRegister("SMPLRT_DIV", readRegister(SMPLRT_DIV));
    debugRegister("CONFIG", readRegister(CONFIG));
    debugRegister("PWR_MGMT_1", readRegister(PWR_MGMT_1));
    debugRegister("GYRO_CONFIG", readRegister(GYRO_CONFIG));
    debugRegister("ACCEL_CONFIG", readRegister(ACCEL_CONFIG));
    digitalWrite(PIN_MPU_A0[m], ACCEL_OFF);
  }
}

void loop() {
  for (int m = 0; m < MPU_COUNT; m++) {
    digitalWrite(PIN_MPU_A0[m], ACCEL_ON);
    const uint8_t BYTES_PER_ACCEL_READING = 6;
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDRESS, BYTES_PER_ACCEL_READING, (uint8_t)true);
  
    int16_t x = Wire.read() << 8;
    x |= Wire.read();
    int16_t y = Wire.read() << 8;
    y |= Wire.read();
    int16_t z = Wire.read() << 8;
    z |= Wire.read();

    Serial.print(m);
    Serial.print(": <");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.print(z);
    Serial.print(">  ");
    digitalWrite(PIN_MPU_A0[m], ACCEL_OFF);
  }
  Serial.println();
  delay(1000);
}
