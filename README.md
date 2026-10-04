# Air Quality Monitoring System

A beginner-friendly, breadboard-based project that monitors temperature, humidity, and gas levels using an OLED display and buzzer alerts.

## What it does
*   **Reads Sensors:** Captures temperature and humidity from a DHT11 sensor and gas levels from an MQ-2 sensor.
*   **Displays Data:** Shows live readings on a 128x64 OLED screen.
*   **Alerts:** Sounds a buzzer if gas levels exceed 40% or if the digital output detects a gas event.

## Hardware
*   **Arduino Uno**
*   **OLED Display** (128x64, SSD1306, I2C)
*   **DHT11** (Temperature & Humidity Sensor)
*   **MQ-2** (Gas Sensor)
*   **Piezo Buzzer**
*   **Resistor** (100 Ohm)
*   **Breadboard** and jumper wires

## Wiring
Connect components to the Arduino Uno and breadboard as follows:

| Component | Pin | Arduino Pin | Breadboard Terminal |
| :--- | :--- | :--- | :--- |
| **OLED** | VCC | 5V | Positive Rail |
| **OLED** | GND | GND | Negative Rail |
| **OLED** | SDA | SDA | |
| **OLED** | SCL | SCL | |
| **DHT11** | VCC | 5V | Positive Rail |
| **DHT11** | GND | GND | Negative Rail |
| **DHT11** | DATA | D2 | |
| **MQ-2** | VCC | 5V | Positive Rail |
| **MQ-2** | GND | GND | Negative Rail |
| **MQ-2** | AOUT | A0 | |
| **MQ-2** | DOUT | D3 | |
| **Buzzer** | + | D4 | |
| **Buzzer** | - | 100Ω Resistor Pin 1 | |
| **Resistor** | Pin 2 | Negative Rail | |

## How to use
1.  **Install Dependencies:** Ensure you have the U8g2 library installed in your Arduino IDE or PlatformIO environment.
2.  **Upload:** Connect the Arduino Uno via USB and upload the code.
3.  **Monitor:**
    *   Open the **Serial Monitor** (115200 baud) to see detailed text data.
    *   Watch the **OLED screen** for real-time graphs of Temperature, Humidity, and Gas percentage.
    *   If gas levels rise above 40%, the **buzzer** will sound.

## Files
*   `platformio.ini` - Project configuration for the build environment.
*   `src/config.h` - Pin definitions and calibration constants.
*   `src/main.cpp` - Main application logic for sensors and display.
*   `schematic/main.sch` - Electronic schematic diagram.
*   `wiring/wiring.json` - Visual breadboard wiring layout.
