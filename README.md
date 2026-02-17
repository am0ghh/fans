# Dorm Room Fan Automation

An Arduino-based temperature-controlled IR automation system that automatically turns on fans when the room gets too hot.

## Project Overview

This project uses an Arduino Uno to monitor room temperature and automatically control multiple IR-enabled fans in a college dorm room. When the temperature exceeds a set threshold, the system sends IR signals to turn on the fans.

## Hardware Components

- **Arduino Uno R3**
- **DHT11 Temperature Sensor**
- **IR Receiver Module (38kHz)** - for capturing remote codes
- **IR LED (940nm)** with PN2222 NPN transistor circuit - for transmitting signals
- **Resistors**: 1kΩ, 100Ω
- **Breadboard and jumper wires**

## Circuit Diagram
![image alt](https://github.com/am0ghh/fans/blob/main/fanswiring.png)

## Circuit Schematic


### IR Transmitter Circuit (with Transistor)
```
Arduino Pin 10 → 1kΩ resistor → PN2222 Base (middle pin)
PN2222 Emitter (right pin) → GND
5V → 100Ω resistor → IR LED (+, long leg)
IR LED (-, short leg) → PN2222 Collector (left pin)
```

### DHT11 Temperature Sensor
```
DHT11 VCC → 5V
DHT11 GND → GND
DHT11 Data → Pin 4
```

### IR Receiver (for code capture)
```
IR Receiver VCC → 5V
IR Receiver GND → GND
IR Receiver Signal → Pin 11
```

## Compatible Fans

This system currently works with:
- ✅ **Woozoo Fan** (NEC Protocol - Code: 0xFF00DE80)
- ✅ **Cosebelle Leafless Fan** (NEC Protocol - Code: 0xFE017F80)


## Libraries Required

```cpp
#include <IRremote.hpp>  // IRremote library by shirriff/ArminJo
#include <DHT.h>         // DHT sensor library
```

Install via Arduino Library Manager or PlatformIO.

## Code Structure

### Capturing IR Codes
Use the receiver code to capture IR signals from fan remotes:
```cpp
#include <Arduino.h>
#include <IRremote.hpp>

const int IR_RECEIVER_PIN = 11;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.print("0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.resume();
  }
}
```

### Main Automation Code
The complete automation code with smart temperature control logic can be found in [`main.cpp`](main.cpp).

**Key features:**
- Temperature hysteresis (ON at 75°F, OFF at 73°F)
- 10-minute minimum runtime to prevent rapid cycling
- DHT11 sensor error handling
- State tracking and serial debugging output

## Key Learnings

### Smart Temperature Control (Hysteresis)
To prevent fans from constantly cycling when temperature hovers around the threshold:
- **Turn ON threshold**: 75°F - fans activate when room gets hot
- **Turn OFF threshold**: 73°F - 2-degree buffer prevents rapid on/off cycling
- **Minimum runtime**: 10 minutes - once fans turn on, they stay on for at least 10 minutes even if temperature drops quickly
- **Error handling**: Checks for invalid sensor readings (NaN) before making decisions

This prevents the annoying situation where temperature bounces between 74-75°F and fans constantly toggle.

### Why the Transistor?
Initially tried using a cheap IR transmitter module, but it lacked sufficient power for reliable transmission. The PN2222 transistor amplifies the Arduino's signal, allowing much higher current through the IR LED for stronger transmission range.

### Protocol Issues
- **NEC Protocol fans** (Woozoo, Cosebelle) work easily with `sendNECRaw()`
- **Unknown/Custom protocols** (Holmes) require capturing and replaying raw timing data
- Some fans use proprietary IR protocols that are difficult to replicate

### IR LED Verification
Use a phone camera to check if IR LEDs are transmitting - they appear as purple/white light on camera screens even though they're invisible to the naked eye.

## Troubleshooting

**Fan doesn't respond:**
- Check IR LED is pointed directly at fan's receiver
- Verify correct hex code with IR receiver
- Test at close range (1-2 feet) first
- Confirm IR LED is transmitting via phone camera

**Inconsistent temperature readings:**
- DHT11 sensors need ~2 second delays between reads
- Add error checking for `NaN` values

**Multiple codes from same button:**
- IR reflections cause noise - look for most frequently occurring code
- Capture codes from close range in stable conditions

## Future Improvements

- [ ] Get Holmes fan working with custom protocol timing
- [ ] Add multiple IR LEDs for better room coverage
- [ ] Implement fan speed control (low/medium/high)
- [ ] Add display for current temperature
- [ ] Make temperature threshold adjustable via button/potentiometer
- [ ] Add oscillation control

## Wiring Notes

The transistor circuit is essential for adequate IR transmission power. Without it, range is limited to ~1-2 feet. With the transistor, range extends to 10-15 feet.

## License

MIT License - feel free to use and modify for your own projects!

## Acknowledgments

Built as a practical solution for automatically cooling a college dorm room. Started as a simple Arduino project and evolved into understanding IR protocols, transistor amplification, and embedded systems troubleshooting.
