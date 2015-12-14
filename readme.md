# Arduino ADC IO Expander

This firmware can be used with any Arduino to turn that board into an (up to\*) 14 Channel, 10-Bit ADC with an I2C interface. 

## Support

The slave board **CANNOT** use the analog inputs that are shared with the `SDA` and `SCL` lines. 

| Board | Analog Inputs | Disallowed Pins | Actual Analog Inputs |
|-------|--------|---------|----------------|
| Arduino Uno | 6 | `A4`, `A5` | 4 (`A0-A3`) |
| Arduino Leonardo | 6 | `-` | 6 (`A0-A5`)|
| Arduino Mega | 6 | `A4`, `A5` | 14 (`A0-A13`) |
| Arduino Micro | 6 | `-` | 6 (`A0-A5`) |
| Arduino Nano | 8 | `A4`, `A5` | 6 (`A0-A3`, `A6-A7`) |
| Arduino Pro Mini | 6 | `A4`, `A5` | 4 (`A0-A3`) |
| Teensy 3.1 | 10 | `A4`, `A5` | 8 (`A0-A3`, `A6-A7`) |


Note: For Leonardo, the analog inputs on the historically digital input ports are not supported.

## I2C Addressing

The default address is `0x04`, with 3 alternative addresses that are configurable via digital pins 4 and 7: 

| Pin 4  | Pin 7  | Address |
|--------|--------|---------|
| `LOW`  | `LOW`  | `0x04`  |
| `LOW`  | `HIGH` | `0x05`  |
| `HIGH` | `LOW`  | `0x06`  |
| `HIGH` | `HIGH` | `0x07`  |



## Commands


| Name      | Byte   | Argument | Description |
|-----------|--------|------------|---------|
| `REPORT`  | `0x7F` | `1` or `0` | Turn on reporting, if this is not set to `1`, analogRead is never called for any enabled pins |
| `ENABLE`  | `0x80` | Number  | Enable an analog pin for reporting, eg. `0` for `A0`, `1` for `A1` and so on. |
| `DISABLE` | `0x81` | Number  | Enable an presently reporting analog pin, eg. `0` for `A0`, `1` for `A1` and so on.  |


## Register

There are two internal registers, one for meta information and one for analog data readings.

#### Register 

| Name  | Address  | Description |
|-------|--------|--------------|
| `NUM_ANALOG_INPUTS` | `0x00` | The number of analog inputs, per board.|
| `A0` | `0xA0` | Analog pin `0` LSB |
| `A1` | `0xA1` | Analog pin `1` LSB |
| `A2` | `0xA2` | Analog pin `2` LSB |
| `A3` | `0xA3` | Analog pin `3` LSB |
| `A4` | `0xA4` | Analog pin `4` LSB \** |
| `A5` | `0xA5` | Analog pin `5` LSB \** |
| `A6` | `0xA6` | Analog pin `6` LSB |
| `A7` | `0xA7` | Analog pin `7` LSB |
| `A8` | `0xA8` | Analog pin `8` LSB |
| `A9` | `0xA9` | Analog pin `9` LSB |
| `A10` | `0xAA` | Analog pin `10` LSB |
| `A11` | `0xAB` | Analog pin `11` LSB |
| `A12` | `0xAC` | Analog pin `12` LSB |
| `A13` | `0xAD` | Analog pin `13` LSB |
| `A14` | `0xAE` | Analog pin `14` LSB |


\** On most platforms, these are disabled. 

## Diagrams

A few diagrams to show example uses: 

![](https://raw.githubusercontent.com/rwaldron/arduino-adc-io-expander/master/diagrams/arduino-uno-to-uno.png)

![](https://raw.githubusercontent.com/rwaldron/arduino-adc-io-expander/master/diagrams/arduino-uno-to-mega.png)

![](https://raw.githubusercontent.com/rwaldron/arduino-adc-io-expander/master/diagrams/arduino-uno-to-micro.png)




## License

See LICENSE-MIT
