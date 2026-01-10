Here's the **complete wiring diagram** for your transmitter with B5K potentiometers:

## **COMPONENTS LIST:**
1. Arduino Nano
2. nRF24L01+ module
3. LCD 16x2 with I2C interface
4. 4x B5K potentiometers (joysticks)
5. 2x Push buttons or toggle switches (AUX1, AUX2)
6. Voltage divider resistors (2x 10kΩ)
7. Breadboard and jumper wires

## **COMPLETE WIRING DIAGRAM:**

### **Arduino Nano Pinout:**
```
ARDUINO NANO:
 ┌─────────────────────────────────────────────┐
 │ [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] │ ← Digital Pins
 │                                             │
 │ [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] │ ← Digital Pins
 │                                             │
 │ [A0] [A1] [A2] [A3] [A4] [A5] [A6] [A7]     │ ← Analog Pins
 └─────────────────────────────────────────────┘
```

### **CONNECTION TABLE:**

| Component | Pin/Wire | Connect To Arduino | Notes |
|-----------|----------|-------------------|-------|
| **nRF24L01+** | | | |
| | CE | D9 | |
| | CSN | D10 | |
| | MOSI | D11 | SPI |
| | MISO | D12 | SPI |
| | SCK | D13 | SPI |
| | VCC | 3.3V | **IMPORTANT: 3.3V NOT 5V!** |
| | GND | GND | |
| **LCD I2C** | | | |
| | GND | GND | |
| | VCC | 5V | |
| | SDA | A4 | |
| | SCL | A5 | |
| **THROTTLE Pot (B5K)** | | | |
| | Pin 1 (Left) | 5V | |
| | Pin 2 (Middle) | A3 | Signal |
| | Pin 3 (Right) | GND | |
| **YAW Pot (B5K)** | | | |
| | Pin 1 (Left) | 5V | |
| | Pin 2 (Middle) | A1 | Signal |
| | Pin 3 (Right) | GND | |
| **PITCH Pot (B5K)** | | | |
| | Pin 1 (Left) | 5V | |
| | Pin 2 (Middle) | A2 | Signal |
| | Pin 3 (Right) | GND | |
| **ROLL Pot (B5K)** | | | |
| | Pin 1 (Left) | 5V | |
| | Pin 2 (Middle) | A6 | Signal |
| | Pin 3 (Right) | GND | |
| **AUX1 Switch** | | | |
| | One side | D5 | |
| | Other side | GND | Active LOW |
| **AUX2 Switch** | | | |
| | One side | D3 | |
| | Other side | GND | Active LOW |
| **Voltage Divider** | | | |
| | Battery+ → R1(10k) → A7 | A7 | |
| | A7 → R2(10k) → GND | GND | |
| | Battery- → GND | GND | |

## **VISUAL WIRING DIAGRAM:**

```
                     ┌─────────────────────────────────────┐
                     │           ARDUINO NANO              │
                     │                                     │
BATTERY + ────R1─────┤ A7    Voltage Monitoring            │
           (10kΩ)    │                                     │
                     │ A6 ────┬──── ROLL Pot (B5K)         │
BATTERY - ───────────┤ GND    │   Pin2: Signal             │
                     │        ├── 5V ── Pin1: 5V           │
                     │        └── GND ─ Pin3: GND          │
                     │                                     │
                     │ A3 ────┬──── THROTTLE Pot (B5K)     │
                     │        │   Pin2: Signal             │
                     │        ├── 5V ── Pin1: 5V           │
                     │        └── GND ─ Pin3: GND          │
                     │                                     │
                     │ A1 ────┬──── YAW Pot (B5K)          │
                     │        │   Pin2: Signal             │
                     │        ├── 5V ── Pin1: 5V           │
                     │        └── GND ─ Pin3: GND          │
                     │                                     │
                     │ A2 ────┬──── PITCH Pot (B5K)        │
                     │        │   Pin2: Signal             │
                     │        ├── 5V ── Pin1: 5V           │
                     │        └── GND ─ Pin3: GND          │
                     │                                     │
                     │ D5 ────────── AUX1 Switch ── GND    │
                     │                                     │
                     │ D3 ────────── AUX2 Switch ── GND    │
                     │                                     │
                     │ A4 ────────── LCD I2C SDA           │
                     │ A5 ────────── LCD I2C SCL           │
                     │ 5V ────────── LCD I2C VCC           │
                     │ GND ───────── LCD I2C GND           │
                     │                                     │
                     │ D9 ────────── nRF24L01 CE           │
                     │ D10 ───────── nRF24L01 CSN          │
                     │ D11 ───────── nRF24L01 MOSI         │
                     │ D12 ───────── nRF24L01 MISO         │
                     │ D13 ───────── nRF24L01 SCK          │
                     │ 3.3V ──────── nRF24L01 VCC          │
                     │ GND ───────── nRF24L01 GND          │
                     └─────────────────────────────────────┘
```

## **POTENTIOMETER (B5K) DETAIL:**

```
B5K LINEAR POTENTIOMETER (Top View):
      ┌─────────────┐
      │    B5K      │
      │  Linear Pot │
      │             │
  ┌───┴─────┬───────┴───┐
  │         │           │
Pin 1      Pin 2      Pin 3
(5V)     (Signal)    (GND)

When joystick moves UP/LEFT: Signal → 5V (1023)
When joystick CENTER: Signal → ~2.5V (~512)
When joystick DOWN/RIGHT: Signal → GND (0)
```

## **VOLTAGE DIVIDER DETAIL:**

```
For 12V Battery Monitoring:
BATTERY+ (12V) ──── R1 (10kΩ) ────┬─── A7
                                  │
                                 R2 (10kΩ)
                                  │
                                GND

Formula: V_at_A7 = Battery_Voltage × (R2/(R1+R2))
For R1=R2=10kΩ: V_at_A7 = Battery_Voltage / 2
So 12V battery → 6V at A7 (but A7 max is 5V!)
```

**Better values for 12V:**
- R1 = 15kΩ
- R2 = 10kΩ
- Then V_at_A7 = 12V × (10k/(15k+10k)) = 4.8V (safe)

## **CRITICAL WIRING TIPS:**

### **1. nRF24L01 Power:**
```
ADD 10µF CAPACITOR:
nRF24L01 VCC ────┬─── 10µF Electrolytic ──── GND
                 │
              3.3V from Arduino
```
This prevents voltage spikes that can damage nRF24L01.

### **2. Separate Power Lines:**
```
ARDUINO 5V ──┬─── LCD
             ├─── Pots
             └─── Voltage Divider

ARDUINO 3.3V ──── nRF24L01 (ONLY!)

ARDUINO GND ──┬─── LCD
              ├─── Pots  
              ├─── nRF24L01
              ├─── Switches
              └─── Voltage Divider
```

### **3. Test Each Connection Step-by-Step:**
1. First connect LCD only - test
2. Connect ONE pot (throttle) - test
3. Connect nRF24L01 - test
4. Add other pots one by one
5. Add switches
6. Add voltage divider

## **COMMON MISTAKES TO AVOID:**

1. **❌ nRF24L01 to 5V** → Will burn module
2. **❌ All pots sharing single wire** → Use separate wires
3. **❌ No capacitor on nRF24L01** → Add 10µF
4. **❌ Wrong pot pin order** → Test with multimeter
5. **❌ Voltage divider without resistors** → Will short circuit

## **QUICK TEST CODE FOR WIRING:**

```cpp
void setup() {
  Serial.begin(9600);
  pinMode(5, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop() {
  Serial.print("Throttle(A3):"); Serial.print(analogRead(A3));
  Serial.print(" Yaw(A1):"); Serial.print(analogRead(A1));
  Serial.print(" Pitch(A2):"); Serial.print(analogRead(A2));
  Serial.print(" Roll(A6):"); Serial.print(analogRead(A6));
  Serial.print(" AUX1:"); Serial.print(digitalRead(5));
  Serial.print(" AUX2:"); Serial.println(digitalRead(3));
  delay(500);
}
```

**Expected output when wiring correct:**
- Moving one joystick affects only its value
- Switches: 1=not pressed, 0=pressed
- All pots show 0-1023 range

## **If Still Having Issues:**

**Use multimeter to check:**
1. 5V at pot pin 1
2. 0V at pot pin 3
3. Variable 0-5V at pin 2 when moving
4. nRF24L01 getting 3.3V

Follow this wiring exactly and your transmitter will work!