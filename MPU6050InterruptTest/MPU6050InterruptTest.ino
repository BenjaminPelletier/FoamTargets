#include <Wire.h>

const uint8_t MPU6050_ADDRESS = 0x68; // Device address when ADO = 0

// MPU-6050 registers and bits (https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf)
const uint8_t ACCEL_CONFIG = 0x1C;
const uint8_t AFS_SEL = 3; // Bits 4:3
const uint8_t AFS_SEL_2G = 0 << AFS_SEL;
const uint8_t AFS_SEL_4G = 1 << AFS_SEL;
const uint8_t AFS_SEL_8G = 2 << AFS_SEL;
const uint8_t AFS_SEL_16G = 3 << AFS_SEL;
const uint8_t ACCEL_HIGHPASS = 0; // Bits 2:0; these 3 bits are grey in the data sheet, but they are used! Leaving them 0 means the filter always outputs 0.
const uint8_t ACCEL_HIGHPASS_5HZ = 1 << ACCEL_HIGHPASS;

const uint8_t MOT_THR = 0x1F; // Units of LSB; 2048 LSB/g @ 16g full-scale, 16384 LSB/g @ 2g full-scale

const uint8_t MOT_DUR = 0x20; // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms

const uint8_t INT_PIN_CFG = 0x37;
const uint8_t INT_LEVEL = 7;
const uint8_t INT_OPEN = 6;
const uint8_t LATCH_INT_EN = 5;
const uint8_t INT_RD_CLEAR = 4;
const uint8_t FSYNC_INT_LEVEL = 3;
const uint8_t FSYNC_INT_EN = 2;
const uint8_t I2C_BYPASS_EN = 1;

const uint8_t INT_ENABLE = 0x38;
const uint8_t MOT_EN = 6;
const uint8_t FIFO_OFLOW_EN = 4;
const uint8_t I2C_MST_INT_EN = 3;
const uint8_t DATA_RDY_EN = 0;

const uint8_t INT_STATUS = 0x3A;

const uint8_t SIGNAL_PATH_RESET = 0x68;
const uint8_t GYRO_RESET = 2;
const uint8_t ACCEL_RESET = 1;
const uint8_t TEMP_RESET = 0;

const uint8_t MOT_DETECT_CTRL = 0x69;

const uint8_t PWR_MGMT_1 = 0x6B;

const uint8_t WHO_AM_I = 0x75; // Should return 0x68
const uint8_t MOT_DETECT_STATUS = 97;

const int PIN_ACCEL_INT = 25;
const int PIN_LED = 2;

void accelWriteRegister(uint8_t slave_register, uint8_t data) {
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDRESS); // Initialize the Tx buffer
  Wire.write(slave_register); // Put slave register address in Tx buffer
  Wire.write(data); // Put data in Tx buffer
  Wire.endTransmission(); // Send the Tx buffer
}

uint8_t accelReadRegister(uint8_t slave_register) {
  const uint8_t ONE_BYTE = 1;
  Wire.beginTransmission(MPU6050_ADDRESS); // Initialize the Tx buffer
  Wire.write(slave_register); // Put slave register address in Tx buffer
  Wire.endTransmission(false); // Send the Tx buffer, but send a restart to keep connection alive
  Wire.requestFrom(MPU6050_ADDRESS, ONE_BYTE); // Read one byte from slave register address 
  return Wire.read(); // Return result
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
  
  pinMode(PIN_ACCEL_INT, INPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  
  Serial.begin(57600);
  Serial.println("Initializing MPU-6050...");

  // Disable all power management and select internal 8MHz oscillator for clock
  accelWriteRegister(PWR_MGMT_1, 0);

  // Reset all internal signal paths
  accelWriteRegister(SIGNAL_PATH_RESET, (1 << GYRO_RESET) | (1 << ACCEL_RESET) | (1 << TEMP_RESET));

  // Hold INT pin high until interrupt is cleared, set interrupt pin to active-high
  accelWriteRegister(INT_PIN_CFG, (1 << LATCH_INT_EN));

  // Set full scale range and highpass filter
  accelWriteRegister(ACCEL_CONFIG, AFS_SEL_2G | ACCEL_HIGHPASS_5HZ);
  
  // Set motion threshold
  accelWriteRegister(MOT_THR, 20);

  // Set motion detect duration
  accelWriteRegister(MOT_DUR, 1);

  //to register 0x69, write the motion detection decrement and a few other settings (for example write 0x15 to set both free-fall and motion decrements to 1 and accelerometer start-up delay to 5ms total by adding 1ms. )   
  accelWriteRegister(MOT_DETECT_CTRL, 0x15);

  // Enable motion detection interrupt only
  accelWriteRegister(INT_ENABLE, (1 << MOT_EN));

  // Read back registers to verify configuration
  debugRegister("WHO_AMI_I", accelReadRegister(WHO_AM_I));
  debugRegister("PWR_MGMT_1", accelReadRegister(PWR_MGMT_1));
  debugRegister("INT_PIN_CFG", accelReadRegister(INT_PIN_CFG));
  debugRegister("ACCEL_CONFIG", accelReadRegister(ACCEL_CONFIG));
  debugRegister("MOT_THR", accelReadRegister(MOT_THR));
  debugRegister("MOT_DUR", accelReadRegister(MOT_DUR));
  debugRegister("MOT_DETECT_CTRL", accelReadRegister(MOT_DETECT_CTRL));
  debugRegister("INT_ENABLE", accelReadRegister(INT_ENABLE));
  debugRegister("MOT_DETECT_STATUS", accelReadRegister(MOT_DETECT_STATUS));

  Serial.println("Initialization complete.");

  digitalWrite(PIN_LED, LOW);
}

void loop() {
  if (digitalRead(PIN_ACCEL_INT) == HIGH) {
    digitalWrite(PIN_LED, HIGH);
    accelReadRegister(0x3A); // Clear interrupt
    //accelReadRegister(0x37);
    digitalWrite(PIN_LED, LOW);
  }
  delay(100);
}
