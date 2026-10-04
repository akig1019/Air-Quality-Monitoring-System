#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "config.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

struct SensorState {
  int temperatureC = 0;
  int humidityPct = 0;
  bool dhtHasValid = false;

  int mqRaw = 0;
  int gasPercent = 0;
  bool mqDigitalAlarm = false;  // true when DOUT is LOW (active alarm)

  bool alarmOn = false;
};

SensorState state;

unsigned long lastDhtReadMs = 0;
unsigned long lastMqReadMs = 0;
unsigned long lastUiUpdateMs = 0;

static bool waitForPinState(uint8_t pin, uint8_t targetState, unsigned long timeoutUs) {
  const unsigned long start = micros();
  while (digitalRead(pin) != targetState) {
    if ((micros() - start) > timeoutUs) {
      return false;
    }
  }
  return true;
}

// Minimal DHT11 reader (integer-only output, checksum-validated)
static bool readDHT11(uint8_t pin, int &humidity, int &temperature) {
  uint8_t data[5] = {0, 0, 0, 0, 0};

  // Start signal
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(20);  // >=18ms
  digitalWrite(pin, HIGH);
  delayMicroseconds(30);
  pinMode(pin, INPUT);

  noInterrupts();

  // Sensor response: LOW (80us), HIGH (80us), then LOW before first bit
  if (!waitForPinState(pin, LOW, 120)) { interrupts(); return false; }
  if (!waitForPinState(pin, HIGH, 120)) { interrupts(); return false; }
  if (!waitForPinState(pin, LOW, 120)) { interrupts(); return false; }

  // Read 40 bits
  for (uint8_t i = 0; i < 40; i++) {
    // Each bit starts with ~50us LOW, then HIGH:
    if (!waitForPinState(pin, HIGH, 80)) { interrupts(); return false; }

    const unsigned long tHighStart = micros();
    if (!waitForPinState(pin, LOW, 120)) { interrupts(); return false; }
    const unsigned long highPulseUs = micros() - tHighStart;

    // ~26-28us => 0, ~70us => 1
    data[i / 8] <<= 1;
    if (highPulseUs > 50) {
      data[i / 8] |= 1;
    }
  }

  interrupts();

  const uint8_t checksum = static_cast<uint8_t>(data[0] + data[1] + data[2] + data[3]);
  if (checksum != data[4]) {
    return false;
  }

  humidity = data[0];
  temperature = data[2];
  return true;
}

static int mqRawToPercent(int raw) {
  long pct = (static_cast<long>(raw) - MQ2_CLEAN_AIR_RAW) * 100L /
             (MQ2_SMOKE_RAW - MQ2_CLEAN_AIR_RAW);
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return static_cast<int>(pct);
}

static void drawDisplay() {
  char line[32];

  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_tr);

    u8g2.drawStr(0, 10, "Air Quality Monitor");

    if (state.dhtHasValid) {
      snprintf(line, sizeof(line), "Temp: %d C", state.temperatureC);
    } else {
      snprintf(line, sizeof(line), "Temp: -- C");
    }
    u8g2.drawStr(0, 24, line);

    if (state.dhtHasValid) {
      snprintf(line, sizeof(line), "Hum : %d %%", state.humidityPct);
    } else {
      snprintf(line, sizeof(line), "Hum : -- %%");
    }
    u8g2.drawStr(0, 36, line);

    snprintf(line, sizeof(line), "Gas : %d %%", state.gasPercent);
    u8g2.drawStr(0, 48, line);

    snprintf(line, sizeof(line), "DO:%s  Alarm:%s",
             state.mqDigitalAlarm ? "GAS" : "OK",
             state.alarmOn ? "ON" : "OFF");
    u8g2.drawStr(0, 60, line);
  } while (u8g2.nextPage());
}

static void printSerial() {
  Serial.print(F("Temp(C): "));
  if (state.dhtHasValid) {
    Serial.print(state.temperatureC);
  } else {
    Serial.print(F("--"));
  }

  Serial.print(F(" | Hum(%): "));
  if (state.dhtHasValid) {
    Serial.print(state.humidityPct);
  } else {
    Serial.print(F("--"));
  }

  Serial.print(F(" | MQ2 Raw: "));
  Serial.print(state.mqRaw);

  Serial.print(F(" | Gas(%): "));
  Serial.print(state.gasPercent);

  Serial.print(F(" | MQ2 DO: "));
  Serial.print(state.mqDigitalAlarm ? F("GAS DETECTED") : F("clear"));

  Serial.print(F(" | Alarm: "));
  Serial.println(state.alarmOn ? F("ON") : F("OFF"));
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_MQ2_DOUT, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  u8g2.setI2CAddress(OLED_I2C_ADDR_7BIT << 1);  // U8g2 expects 8-bit form
  u8g2.begin();

  drawDisplay();
}

void loop() {
  const unsigned long now = millis();

  if (now - lastMqReadMs >= MQ_READ_INTERVAL_MS) {
    lastMqReadMs = now;

    state.mqRaw = analogRead(PIN_MQ2_AOUT);                // 0..1023 on Uno
    state.gasPercent = mqRawToPercent(state.mqRaw);        // 0..100%
    state.mqDigitalAlarm = (digitalRead(PIN_MQ2_DOUT) == LOW); // active LOW
  }

  if (now - lastDhtReadMs >= DHT_READ_INTERVAL_MS) {
    lastDhtReadMs = now;

    int h = 0, t = 0;
    if (readDHT11(PIN_DHT11_DATA, h, t)) {
      state.humidityPct = h;
      state.temperatureC = t;
      state.dhtHasValid = true;
    }
  }

  state.alarmOn = state.mqDigitalAlarm || (state.gasPercent > GAS_PERCENT_ALARM_THRESHOLD);
  digitalWrite(PIN_BUZZER, state.alarmOn ? HIGH : LOW); // active buzzer

  if (now - lastUiUpdateMs >= UI_UPDATE_INTERVAL_MS) {
    lastUiUpdateMs = now;
    drawDisplay();
    printSerial();
  }
}
