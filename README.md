# 🎮 Arduino RC Transmitter with nRF24L01

A professional-grade 6-channel RC transmitter using Arduino, nRF24L01 module, and I2C LCD display. Perfect for drone, robot, or remote-controlled vehicle projects.

![Transmitter Schematic](https://img.shields.io/badge/Arduino-Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white) ![nRF24L01](https://img.shields.io/badge/nRF24L01-2.4GHz-blue?style=for-the-badge) ![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

## 📋 Features

- **6-Channel Control**: Throttle, Yaw, Pitch, Roll + 2 AUX channels
- **Real-time Display**: I2C LCD shows potentiometer readings and system status
- **Smart Scrolling**: 4 different display modes cycle automatically
- **Battery Monitoring**: Built-in voltage divider for battery check
- **Dual Toggle Switches**: 3-position switches for AUX1/AUX2
- **Joystick Buttons**: Integrated push-button controls
- **Professional Mapping**: Custom joystick value calibration
- **Serial Debug**: Optional serial monitor output

## 🛠️ Hardware Requirements

### Components Needed:
| Component | Quantity | Purpose |
|-----------|----------|---------|
| Arduino Uno/Nano | 1 | Main controller |
| nRF24L01+ Module | 1 | 2.4GHz wireless communication |
| I2C LCD (16x2) | 1 | Display interface |
| Potentiometer (10K) | 1 | AUX3 control |
| Analog Joysticks | 2 | Primary controls (X/Y each) |
| Toggle Switches (3-way) | 2 | AUX1/AUX2 channels |
| Push Buttons | 2 | Joystick integrated switches |
| Voltage Divider (10K+10K) | 1 | Battery monitoring |
| 9V Battery | 1 | Power supply |

### Pin Configuration:
| Arduino Pin | Connected To | Function |
|-------------|-------------|----------|
| A0 | Potentiometer | AUX3 analog input |
| A1 | Right Joystick X | Yaw control |
| A2 | Right Joystick Y | Pitch control |
| A3 | Left Joystick X | Throttle control |
| A6 | Left Joystick Y | Roll control |
| A7 | Voltage Divider | Battery monitoring |
| D2 | Toggle Switch 2 Pin 2 | AUX2 position 2 |
| D3 | Toggle Switch 2 Pin 1 | AUX2 position 1 |
| D4 | Toggle Switch 1 Pin 1 | AUX1 position 1 |
| D5 | Toggle Switch 1 Pin 2 | AUX1 position 2 |
| D6 | Right Joystick SW | Button 2 |
| D7 | Left Joystick SW | Button 1 |
| D9 | nRF24L01 CSN | SPI chip select |
| D10 | nRF24L01 CE | Chip enable |
| D11 | nRF24L01 MOSI | SPI data |
| D12 | nRF24L01 MISO | SPI data |
| D13 | nRF24L01 SCK | SPI clock |
| SDA (A4) | I2C LCD SDA | Display data |
| SCL (A5) | I2C LCD SCL | Display clock |

## 📦 Software Installation

### 1. Install Required Libraries
```cpp
// Required Libraries (install via Arduino Library Manager):
#include <SPI.h>          // Built-in
#include <nRF24L01.h>     // RF24 by TMRh20
#include <RF24.h>         // RF24 by TMRh20
#include <LiquidCrystal_I2C.h> // Frank de Brabander
```

### 2. Upload the Code
1. Open Arduino IDE (v1.8+)
2. Install the required libraries
3. Copy the transmitter code into a new sketch
4. Select your Arduino board and port
5. Click **Upload**

### 3. Receiver Setup
A matching receiver code is needed for your drone/robot. The receiver should:
- Use same nRF24L01 module
- Listen on address `0xE8E8F0F0E1`
- Process the 8-byte data structure

## 🔧 Calibration & Setup

### Joystick Calibration
The code includes automatic calibration for joystick centering. For manual calibration:

```cpp
// Adjust these values in mapJoystickValues() function:
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
// lower: Minimum reading (joystick fully left/up)
// middle: Center position reading
// upper: Maximum reading (joystick fully right/down)
```

### LCD Address
The I2C LCD address is set to `0x27`. If your display doesn't work:
```cpp
// Common alternatives:
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Try 0x3F if this fails
```

### Potentiometer Scaling
Adjust the potentiometer mapping if needed:
```cpp
// Current mapping: 0-1023 → 0-255
potMappedValue = map(potRawValue, 0, 1023, 0, 255);
// Change to: map(potRawValue, minReading, maxReading, 0, 255);
```

## 📊 Display Modes

The LCD automatically cycles through 4 informative displays:

1. **Main Potentiometer** - Shows current AUX3 value (0-255)
2. **Raw Details** - Displays raw ADC and percentage values
3. **Battery + Status** - Voltage reading and switch states
4. **Joystick Values** - All 4 primary channel values

## 🔌 Wiring Diagram

```
┌─────────────────────────────────────────────────────────┐
│                     ARDUINO UNO                         │
├─────────────────────────────────────────────────────────┤
│      nRF24L01      │     I2C LCD      │   Joysticks     │
│  CE  ──── D10      │  SDA ──── A4     │  Left X ─ A3    │
│  CSN ──── D9       │  SCL ──── A5     │  Left Y ─ A6    │
│  MOSI ─── D11      │                  │  Right X ─ A1   │
│  MISO ─── D12      │     Switches     │  Right Y ─ A2   │
│  SCK ──── D13      │  T1-P1 ─ D4      │                  │
│                    │  T1-P2 ─ D5      │    Buttons      │
│   Voltage Divider  │  T2-P1 ─ D3      │  Left SW ─ D7   │
│   Vbat ─── A7      │  T2-P2 ─ D2      │  Right SW ─ D6  │
│                    │                  │                  │
│   Potentiometer    │                  │                  │
│   AUX3 ─── A0      │                  │                  │
└─────────────────────────────────────────────────────────┘
```

## 📡 nRF24L01 Configuration

```cpp
// RF24 Settings (optimized for reliability):
radio.setDataRate(RF24_250KBPS);  // Slower = more reliable
radio.setPALevel(RF24_PA_MAX);    // Maximum power
radio.setAutoAck(false);          // Disable auto-acknowledge
```

## 🚀 Usage Instructions

1. **Power On**: Connect 9V battery to Arduino
2. **Initialization**: Wait for "POTENTIOMETER READING ACTIVE" message
3. **Calibrate**: Ensure joysticks are centered before use
4. **Monitor**: Watch LCD for real-time feedback
5. **Transmit**: Data is automatically sent to receiver
6. **Debug**: Open Serial Monitor (9600 baud) for detailed output

## 📈 Serial Monitor Output

The transmitter provides verbose serial output every second:
```
Pot - Raw:512 Map:128 %:50 Bat:8.4V | T:127 Y:128 P:126 R:129
```

## ⚠️ Troubleshooting

| Problem | Solution |
|---------|----------|
| LCD not displaying | Check I2C address (try 0x27 or 0x3F) |
| No wireless signal | Verify nRF24L01 wiring, check power (3.3V) |
| Joystick not centered | Adjust middle values in mapJoystickValues() |
| Potentiometer jumps | Add capacitor (10µF) across potentiometer |
| Battery reading wrong | Recalculate voltage divider ratio |
| Buttons not working | Check pull-up resistors (code uses INPUT_PULLUP) |

## 🔄 Customization

### Adding More Channels
```cpp
// In MyData struct:
byte AUX5;
byte AUX6;

// Read additional controls
data.AUX5 = analogRead(A8);  // If using Arduino Mega
```

### Changing Display Intervals
```cpp
// Adjust timing in global variables:
const unsigned long lcdUpdateInterval = 300;   // Faster updates
const unsigned long displayChangeInterval = 5000; // Slower cycling
```

### Changing RF Settings
```cpp
// For different RF24 settings:
radio.setChannel(76);        // Change frequency channel
radio.setDataRate(RF24_2MBPS); // Faster data rate
radio.setRetries(15, 15);    // Retry configuration
```

## 📄 Data Structure

The transmitter sends this 8-byte structure:
```cpp
struct MyData {
  byte throttle;  // Left joystick X: 0-255
  byte yaw;       // Right joystick X: 0-255
  byte pitch;     // Right joystick Y: 0-255
  byte roll;      // Left joystick Y: 0-255
  byte AUX1;      // Toggle switch 1: 0,1,2
  byte AUX2;      // Toggle switch 2: 0,1,2
  byte AUX3;      // Potentiometer: 0-255
  byte AUX4;      // Joystick buttons: bitmask
};
```

## 🛡️ Safety Notes

1. **Power Supply**: Use stable 9V power source
2. **Antenna**: Keep nRF24L01 antenna away from metal
3. **Range Test**: Test in open area before actual use
4. **Interference**: Avoid 2.4GHz crowded environments
5. **Battery**: Monitor voltage to prevent brownouts

## 📚 Resources & References

- [nRF24L01 Datasheet](https://www.nordicsemi.com/products/nrf24-series)
- [RF24 Library Documentation](https://github.com/nRF24/RF24)
- [Arduino SPI Reference](https://www.arduino.cc/en/Reference/SPI)
- [I2C LCD Library](https://github.com/johnrickman/LiquidCrystal_I2C)

## 👥 Contributing

Found a bug or have an improvement? 
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## 📄 License

MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **TMRh20** for the excellent RF24 library
- **Arduino Community** for endless inspiration
- **All open-source contributors** who make projects like this possible

## 🌟 Star History

If you find this project useful, please give it a star! ⭐

---

**Made with ❤️ by Kalyxon** | [View on GitHub](https://github.com/kalyxon) | *Happy flying!* 🚁

---

*Last Updated: $(date +%Y-%m-%d)*

*Project Version: 2.0*
