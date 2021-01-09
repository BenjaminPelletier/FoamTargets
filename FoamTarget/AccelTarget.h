#ifndef AccelTarget_h
#define AccelTarget_h

#include <Wire.h>

const uint8_t MPU6050_ADDRESS = 0x69; // Device address when ADO = 1

// MPU-6050 registers and bits
const uint8_t SMPLRT_DIV = 0x19;

const uint8_t CONFIG = 0x1A;

const uint8_t GYRO_CONFIG = 0x1B;

const uint8_t ACCEL_CONFIG = 0x1C;
const uint8_t AFS_SEL = 3; // Bits 4:3
const uint8_t AFS_SEL_2G = 0 << AFS_SEL;
const uint8_t AFS_SEL_4G = 1 << AFS_SEL;
const uint8_t AFS_SEL_8G = 2 << AFS_SEL;
const uint8_t AFS_SEL_16G = 3 << AFS_SEL;
const uint8_t ACCEL_HIGHPASS = 0; // Bits 2:0; these 3 bits are grey in the data sheet, but they are used! Leaving them 0 means the filter always outputs 0.
const uint8_t ACCEL_HIGHPASS_5HZ = 1 << ACCEL_HIGHPASS;

const uint8_t INT_PIN_CFG = 0x37;
const uint8_t INT_LEVEL = 7;
const uint8_t INT_OPEN = 6;
const uint8_t LATCH_INT_EN = 5;
const uint8_t INT_RD_CLEAR = 4;
const uint8_t FSYNC_INT_LEVEL = 3;
const uint8_t FSYNC_INT_EN = 2;
const uint8_t I2C_BYPASS_EN = 1;

const uint8_t ACCEL_XOUT_H = 0x3B;

const uint8_t SIGNAL_PATH_RESET = 0x68;
const uint8_t GYRO_RESET = 2;
const uint8_t ACCEL_RESET = 1;
const uint8_t TEMP_RESET = 0;

const uint8_t PWR_MGMT_1 = 0x6B;

const uint8_t WHO_AM_I = 0x75; // Should return 0x68
const uint8_t MPU6050_ID = 0x68;
const uint8_t MPU6050_INVALID_ID = 0xFF;

// Accelerometer trigger characteristics
const float BASELINE_UPDATE_WEIGHT = 0.1f;
const float BASELINE_PRIOR_WEIGHT = 1.0f - BASELINE_UPDATE_WEIGHT;
const float BASELINE_TOLERANCE = 6000.0f;
const int DEBOUNCE_MS = 80;
const int RECALIBRATION_DELAY_MS = 5000;

const int ACCEL_OFF = LOW;
const int ACCEL_ON = HIGH;

const uint8_t MEASURE_NO_CHANGE = 0;
const uint8_t MEASURE_BECAME_ACTIVE = 1;
const uint8_t MEASURE_BECAME_INACTIVE = 2;

class AccelTarget {
  private:
    float baseline;
    unsigned long lastNominal;

    String debugRegister(String register_name, uint8_t register_value);
    void writeRegister(uint8_t slave_register, uint8_t value);
    uint8_t readRegister(uint8_t slave_register);
    void readAccelerometer();

  public:
    unsigned long lastDeviation;
  
    int pinA0;
    bool valid;
    int16_t x;
    int16_t y;
    int16_t z;
    bool active;
    
    int count;
    float maxDelta;

    void configure(int pin_a0);
    void init(unsigned long t);
    uint8_t measure(unsigned long t);
    String debug();
};

String AccelTarget::debugRegister(String register_name, uint8_t register_value) {
  String response = "0b";
  uint8_t v = register_value;
  for (uint8_t i = 0; i < 8; i++) {
    response += v >= 0x80 ? '1' : '0';
    v <<= 1;
  }
  response += " 0x";
  v = register_value >> 4;
  response += (char)((v <= 9 ? '0' : '7') + v);
  v = register_value & 0x0F;
  response += (char)((v <= 9 ? '0' : '7') + v);
  response += ' ';
  response += register_value;
  response += ' ';
  response += register_name;
  response += '\n';
  return response;
}

void AccelTarget::writeRegister(uint8_t slave_register, uint8_t value) {
  Wire.beginTransmission(MPU6050_ADDRESS); // Initialize the Tx buffer
  Wire.write(slave_register); // Put slave register address in Tx buffer
  Wire.write(value); // Put data in Tx buffer
  Wire.endTransmission(); // Send the Tx buffer
}

uint8_t AccelTarget::readRegister(uint8_t slave_register) {
  const uint8_t ONE_BYTE = 1;
  Wire.beginTransmission(MPU6050_ADDRESS); // Initialize the Tx buffer
  Wire.write(slave_register); // Put slave register address in Tx buffer
  Wire.endTransmission(false); // Send the Tx buffer, but send a restart to keep connection alive
  Wire.requestFrom(MPU6050_ADDRESS, ONE_BYTE); // Read one byte from slave register address 
  return Wire.read(); // Return result
}

void AccelTarget::configure(int pin_a0) {
  pinA0 = pin_a0;
  pinMode(pinA0, OUTPUT);
  digitalWrite(pinA0, ACCEL_OFF);
}

void AccelTarget::init(unsigned long t) {
  digitalWrite(pinA0, ACCEL_ON);

  for (uint8_t attempt = 0; attempt < 5; attempt++) {
    writeRegister(SMPLRT_DIV, 0x00);
  
    writeRegister(CONFIG, 0x00);
  
      // Set full scale range
    writeRegister(ACCEL_CONFIG, AFS_SEL_16G);
    
    // Disable all power management and select internal 8MHz oscillator for clock
    writeRegister(PWR_MGMT_1, 0);
  
    // Reset all internal signal paths
    writeRegister(SIGNAL_PATH_RESET, (1 << GYRO_RESET) | (1 << ACCEL_RESET) | (1 << TEMP_RESET));
  
    // Verify that we're talking to a working accelerometer
    valid = readRegister(WHO_AM_I) != MPU6050_INVALID_ID;
  
    if (valid) {
      // Set initial state values
      readAccelerometer();
    
      baseline = z;
      active = false;
      lastNominal = t;
      lastDeviation = t;
      break;
    }

    delay(100);
  }

  digitalWrite(pinA0, ACCEL_OFF);
}

String AccelTarget::debug() {
  // Read back registers to verify configuration
  digitalWrite(pinA0, ACCEL_ON);
  String response = debugRegister("WHO_AM_I", readRegister(WHO_AM_I));
  response += debugRegister("SMPLRT_DIV", readRegister(SMPLRT_DIV));
  response += debugRegister("CONFIG", readRegister(CONFIG));
  response += debugRegister("PWR_MGMT_1", readRegister(PWR_MGMT_1));
  response += debugRegister("GYRO_CONFIG", readRegister(GYRO_CONFIG));
  response += debugRegister("ACCEL_CONFIG", readRegister(ACCEL_CONFIG));
  digitalWrite(pinA0, ACCEL_OFF);
  return response;
}

void AccelTarget::readAccelerometer() {
    digitalWrite(pinA0, ACCEL_ON);
    
    const uint8_t BYTES_PER_ACCEL_READING = 6;
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDRESS, BYTES_PER_ACCEL_READING, (uint8_t)true);

    x = Wire.read() << 8;
    x |= Wire.read();
    y = Wire.read() << 8;
    y |= Wire.read();
    z = Wire.read() << 8;
    z |= Wire.read();

    digitalWrite(pinA0, ACCEL_OFF);
}

uint8_t AccelTarget::measure(unsigned long t) {
  if (!valid) {
    return MEASURE_NO_CHANGE;
  }
  
  readAccelerometer();
  
  uint8_t result = MEASURE_NO_CHANGE;
  float dz = abs(z - baseline);
  if (dz > BASELINE_TOLERANCE) {
    if (!active) {
      active = true;
      result = MEASURE_BECAME_ACTIVE;
      count++;
    }
    if (dz > maxDelta) maxDelta = dz; if (-dz > maxDelta) maxDelta = -dz;
    lastDeviation = t;
  } else if (t > lastDeviation + DEBOUNCE_MS) {
    if (active) {
      active = false;
      result = MEASURE_BECAME_INACTIVE;
    }
    lastNominal = t;
  }
  
  if (!active || t - lastNominal > RECALIBRATION_DELAY_MS) {
    baseline = BASELINE_PRIOR_WEIGHT * baseline + BASELINE_UPDATE_WEIGHT * z;
  }
  
  return result;
}

#endif
