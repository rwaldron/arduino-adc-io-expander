#include <Boards.h>
#include <Wire.h>

#if defined(__AVR_ATmega32U4__) && TOTAL_ANALOG_PINS == 12
#define NUM_ALLOWED_ANALOG_PINS 6
#else
#define NUM_ALLOWED_ANALOG_PINS TOTAL_ANALOG_PINS - 2
#endif

#define ANALOG_PIN_OFFSET TOTAL_PINS - TOTAL_ANALOG_PINS

#define DEBUG_MODE 0
#define MAX_BUFFER_LENGTH 32

#define I2C_ADDRESS_BASE 0x04
#define I2C_COMMAND_BASE 0x7F
#define I2C_REGISTER_BASE 0xA0

#define COMMAND_REPORT 0x7F
#define COMMAND_ENABLE 0x80
#define COMMAND_DISABLE 0x81


int addressBits = 0;
/*
  Computed by adding the value of addressBits
  to the I2C_ADDRESS_BASE (0x04)
*/
int i2cAddress = 0;

const int i2cDataBufferLength = 32;
uint8_t i2cDataRegister[i2cDataBufferLength];
/*
  Data Register
  {
    TOTAL_ANALOG_PINS,
    Unused,
    2, A0 (LSB)
    3, A0 (MSB)
    4, A1 (LSB)
    5, A1 (MSB)
    6, A2 (LSB)
    7, A2 (MSB)
    ...
  }
*/

int receivedIndex = 0;
int i2cDataRegisterIndex = 0;
/*
  0xA0 => 160
  0xA1 => 161
  0xA2 => 162
  0xA3 => 163
  0xA4 => 164
  0xA5 => 165
  0xA6 => 166
  0xA7 => 167
  0xA8 => 168
*/

uint16_t analogReadEnabled = 0;
int isReporting = 0;

void setup() {

  #if DEBUG_MODE
  Serial.begin(9600);
  Serial.println("[setup]");
  #endif

  #if DEBUG_MODE
  Serial.print("NUM_ALLOWED_ANALOG_PINS: ");
  Serial.println(NUM_ALLOWED_ANALOG_PINS);
  Serial.print("ANALOG_PIN_OFFSET: ");
  Serial.println(ANALOG_PIN_OFFSET);
  #endif


  i2cDataRegister[0] = (uint8_t)NUM_ALLOWED_ANALOG_PINS;

  #if DEBUG_MODE
  Serial.print("NUM_ALLOWED_ANALOG_PINS: ");
  Serial.println(NUM_ALLOWED_ANALOG_PINS);
  Serial.print("ANALOG_PIN_OFFSET: ");
  Serial.println(i2cDataRegister[0]);
  #endif

  /*
  | Pin 2  | Pin 4  | Address |
  |--------|--------|---------|
  | `LOW   | `LOW`  | `0x04`  |
  | `LOW   | `HIGH` | `0x05`  |
  | `HIGH` | `LOW`  | `0x06`  |
  | `HIGH` | `HIGH` | `0x07`  |
  */
  pinMode(0, INPUT);
  pinMode(1, INPUT);

  int dr0 = digitalRead(2);
  int dr1 = digitalRead(4);

  addressBits |= (dr0 << 0);
  addressBits |= (dr1 << 1);

  i2cAddress = I2C_ADDRESS_BASE + addressBits;

  #if DEBUG_MODE
  Serial.print("dr0: ");
  Serial.println(dr0);

  Serial.print("dr1: ");
  Serial.println(dr1);

  Serial.print("addressBits: ");
  Serial.println(addressBits);

  Serial.print("i2cAddress: ");
  Serial.println(i2cAddress);
  #endif

  Wire.begin(i2cAddress);
  Wire.onRequest(onRequest);
  Wire.onReceive(onReceive);
}

void loop() {
  if (isReporting) {
    for (int i = 0; i < NUM_ALLOWED_ANALOG_PINS; i++) {
      if (IS_PIN_I2C(i + ANALOG_PIN_OFFSET) ||
          !(analogReadEnabled & (1 << i))) {
        continue;
      }

      int lsbIndex = (i * 2) + 2;
      int msbIndex = lsbIndex + 1;
      int value = analogRead(i);

      i2cDataRegister[lsbIndex] = value & 0xFF;
      i2cDataRegister[msbIndex] = (value >> 8) & 0xFF;
    }
  }
}

void onRequest() {
  Serial.println("[onRequest]");

  if (receivedIndex == i2cDataRegisterIndex) {
    #if DEBUG_MODE
    Serial.println("(meta)");
    Serial.print("Sending: ");
    Serial.println((byte)i2cDataRegister[receivedIndex]);
    #endif

    Wire.write((byte)i2cDataRegister[receivedIndex]);
  } else {
    uint8_t buffer[i2cDataBufferLength];

    // #if DEBUG_MODE
    // Serial.println("(data)");
    // Serial.print("Sending: ");
    // Serial.println(bSize);
    // #endif

    for (int i = 0; i < i2cDataBufferLength; i++) {
      buffer[i] = i2cDataRegister[i2cDataRegisterIndex + i];
      // #if DEBUG_MODE
      // Serial.print(bIndex);
      // Serial.print(": ");
      // Serial.println(buffer[bIndex]);
      // #endif
    }
    Wire.write(buffer, i2cDataBufferLength);
  }
}

void onReceive(int byteCount) {
  #if DEBUG_MODE
  Serial.println("[onReceive]");
  Serial.print("byteCount: ");
  Serial.println(byteCount);
  #endif

  byte received[byteCount];

  for (int i = 0; i < byteCount; i++) {
    received[i] = Wire.read();

    #if DEBUG_MODE
    Serial.print(i);
    Serial.print(": ");
    Serial.println(received[i]);
    #endif
  }

  if (received[0] >= I2C_COMMAND_BASE &&
      received[0] < I2C_REGISTER_BASE) {
    int command = received[0];
    int arg = received[1];

    if (command == COMMAND_REPORT) {
      isReporting = arg;
      #if DEBUG_MODE
      Serial.println("COMMAND_REPORT");
      Serial.println(arg);
      #endif
    }

    if (command == COMMAND_ENABLE) {
      analogReadEnabled |= 1 << arg;
      #if DEBUG_MODE
      Serial.println("COMMAND_ENABLE");
      Serial.println(arg);
      #endif
    }

    if (command == COMMAND_DISABLE) {
      analogReadEnabled &= ~(1 << arg);
      #if DEBUG_MODE
      Serial.println("COMMAND_DISABLE");
      Serial.println(arg);
      #endif
    }
    #if DEBUG_MODE
    Serial.println("----------------------------------------------------------------");
    #endif
  } else {

    receivedIndex = received[0];

    i2cDataRegisterIndex = receivedIndex >= I2C_REGISTER_BASE ?
      ((receivedIndex - I2C_REGISTER_BASE) * 2) + 2 :
      receivedIndex;

    #if DEBUG_MODE
    Serial.print("receivedIndex: ");
    Serial.println(receivedIndex);
    Serial.print("i2cDataRegisterIndex: ");
    Serial.println(i2cDataRegisterIndex);
    #endif
  }
}
