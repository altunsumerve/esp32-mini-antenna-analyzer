// SPDX-License-Identifier: GPL-3.0-or-later
// Standalone BENCH sketch. Read ../../ANTEN-TEST.md before connecting GPIO34.
// GPIO34 measures the external diode detector's DC, NEVER Si5351 RF directly.
#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "src/Si5351/si5351.h"
#include "scan_config.h"

constexpr uint32_t XTAL_HZ = 25000000; // Change to 27000000 for a 27 MHz module.
constexpr uint8_t DET_PIN = 34;
constexpr uint32_t SETTLE_MS = 180, HOLD_MS = 60000;
constexpr uint32_t LIVE_HOLD_MS = 300000;  // live sweep: auto OFF after 5 min
constexpr uint8_t LIVE_SETTLE_MS = 8;       // detector RC (R3*C3 = 2 ms) settling per point
Si5351 synth;
bool ready = false, scanning = false, rfOn = false, live = false, listening = false;
uint8_t phase = 0;
uint32_t hz = 7100000, lastHz, stepHz, phaseAt, holdAt, reportAt;
DetectorReading baseline;
float bestDelta;
uint32_t bestHz, firstHz;
bool anyClip;
char command[96];
size_t used = 0;
bool tooLong = false;
void pollCommands();

// A successful address ACK alone is insufficient: read status with a byte count.
bool clockHealthy() {
  Wire.beginTransmission(0x60);
  Wire.write(uint8_t(0));
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(uint8_t(0x60), uint8_t(1)) != 1) return false;
  return (Wire.read() & 0xB0) == 0; // SYS_INIT, LOL_A, LOS; PLLB is unused.
}

void outputsOff() {
  if (ready) {
    synth.output_enable(SI5351_CLK0, 0);
    synth.output_enable(SI5351_CLK1, 0);
    synth.output_enable(SI5351_CLK2, 0);
  }
  rfOn = false;
}

void stopTest() {
  scanning = false;
  live = false;
  listening = false;
  outputsOff();
}

bool tune(uint32_t f) {
  if (ready) synth.set_clock_pwr(SI5351_CLK0, 0);
  if (!ready || synth.set_freq(uint64_t(f) * 100ULL, SI5351_CLK0) != 0) {
    stopTest();
    Serial.println("# ERROR: Si5351 not ready; RF off requested.");
    return false;
  }
  delay(10);
  if (!clockHealthy()) {
    stopTest();
    Serial.println("# ERROR: clock/I2C fault; unplug USB if module is disconnected.");
    return false;
  }
  synth.output_enable(SI5351_CLK0, 1);
  synth.set_clock_pwr(SI5351_CLK0, 1);
  rfOn = true;
  return true;
}

DetectorReading readDetector() {
  uint32_t sum = 0;
  DetectorReading r{0, 4095, 0};
  for (unsigned i = 0; i < 64; ++i) {
    uint16_t v = analogRead(DET_PIN);
    sum += v;
    if (v < r.low) r.low = v;
    if (v > r.high) r.high = v;
    delayMicroseconds(500);
  }
  r.mean = sum / 64.0f;
  return r;
}

// Fast reading for the live sweep (16 samples).
float readDetectorFast() {
  uint32_t sum = 0;
  for (unsigned i = 0; i < 16; ++i) { sum += analogRead(DET_PIN); delayMicroseconds(200); }
  return sum / 16.0f;
}

// One live sweep: baseline with RF off, then every point with RF on.
// Output: "L,<startHz>,<stepHz>,<delta1>,<delta2>,..."  (delta = raw - baseline)
void liveSweep() {
  outputsOff();
  delay(LIVE_SETTLE_MS * 3);
  float base = readDetectorFast();
  Serial.printf("L,%lu,%lu", (unsigned long)firstHz, (unsigned long)stepHz);
  for (uint32_t f = firstHz; f <= lastHz; f += stepHz) {
    if (!tune(f)) return;                // tune() stops everything on error
    delay(LIVE_SETTLE_MS);
    Serial.printf(",%.0f", readDetectorFast() - base);
    pollCommands();                      // allow "off" in the middle of a sweep
    if (!live) { Serial.println(); outputsOff(); return; }
  }
  Serial.println();
}

void help() {
  Serial.println("# ANTEN_TEST: 6-8 MHz; diode DC detector -> GPIO34; 115200/Newline");
  Serial.println("# f7100000 : fixed RF + raw ADC readings; auto OFF after 60 s");
  Serial.println("# scan 6800000 7400000 10000 : Hz start/end/step (<=401 points)");
  Serial.println("# live 7200000 8000000 20000 : endless sweep for canli-grafik.html; auto OFF after 5 min");
  Serial.println("# listen : RF OFF, stream detector level (e.g. phone NFC 13.56 MHz); auto OFF after 5 min");
  Serial.println("# off : stop now; read : raw ADC; help : commands");
  Serial.println("# Scan auto OFF. Raw counts are NOT dBm, SWR, Q or proof of resonance.");
}

void runCommand(char *line) {
  char *arg[5];
  size_t n = 0;
  for (char *p = strtok(line, " \t"); p && n < 5; p = strtok(nullptr, " \t")) arg[n++] = p;
  if (!n) return;
  if (n == 1 && !strcmp(arg[0], "help")) { help(); return; }
  if (n == 1 && !strcmp(arg[0], "off")) { stopTest(); Serial.println("# OFF requested"); return; }
  if (n == 1 && !strcmp(arg[0], "read")) {
    auto r = readDetector();
    Serial.printf("# ADC raw=%.1f min=%u max=%u RF=%s\n", r.mean, r.low, r.high, rfOn ? "ON" : "OFF");
    return;
  }
  stopTest();
  uint32_t a, b, c;
  if (n == 1 && arg[0][0] == 'f' && parseHz(arg[0] + 1, a) && a >= MIN_HZ && a <= MAX_HZ) {
    hz = a;
    if (tune(hz)) {
      holdAt = reportAt = millis();
      Serial.println("# Fixed RF ON: auto OFF in 60 seconds. Turn knob; then type off.");
    }
    return;
  }
  if (n == 4 && !strcmp(arg[0], "scan") && parseHz(arg[1], a) && parseHz(arg[2], b) &&
      parseHz(arg[3], c) && validScan(a, b, c) && ready) {
    firstHz = hz = a; lastHz = b; stepHz = c;
    bestDelta = -4096; bestHz = a; anyClip = false;
    scanning = true; phase = 0; phaseAt = millis();
    Serial.println("# Freeze antenna, cable and tuning knob; off cancels.");
    Serial.println("hz,off_raw,on_raw,delta_raw,on_min,on_max,off_min,off_max");
    return;
  }
  if (n == 1 && !strcmp(arg[0], "listen")) {
    // Si5351 stays OFF: the loop only receives (e.g. a phone's NFC field).
    listening = true; holdAt = reportAt = millis();
    Serial.println("# LISTEN: RF OFF. Output R,<mean>,<max> every 100 ms. off stops.");
    return;
  }
  if (n == 4 && !strcmp(arg[0], "live") && parseHz(arg[1], a) && parseHz(arg[2], b) &&
      parseHz(arg[3], c) && validScan(a, b, c) && ready) {
    firstHz = a; lastHz = b; stepHz = c;
    live = true; holdAt = millis();
    Serial.println("# LIVE sweep ON (RF on while sweeping). off stops; auto OFF after 5 min.");
    return;
  }
  Serial.println("# ERROR: invalid command/range or module absent. RF off requested. Type help.");
}

void pollCommands() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') {
      if (tooLong) { stopTest(); Serial.println("# ERROR: line too long; OFF requested"); }
      else if (used) { command[used] = 0; runCommand(command); }
      used = 0; tooLong = false;
    } else if (used < sizeof(command) - 1 && !tooLong) command[used++] = c;
    else tooLong = true;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  Wire.setTimeOut(50);
  analogReadResolution(12);
  analogSetPinAttenuation(DET_PIN, ADC_11db);
  ready = synth.init(SI5351_CRYSTAL_LOAD_8PF, XTAL_HZ, 0);
  if (ready) {
    synth.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
    outputsOff();
  }
  help();
  Serial.println(ready ? "# READY; RF OFF. Check detector DC before connecting GPIO34." : "# ERROR: Si5351 absent/init timeout. Check power/I2C and reset.");
}

void loop() {
  pollCommands();
  if (listening) {
    if (millis() - holdAt >= LIVE_HOLD_MS) { stopTest(); Serial.println("# AUTO OFF after 5 min"); }
    else if (millis() - reportAt >= 100) {
      // NFC readers transmit in short bursts: report the peak as well as the mean.
      uint32_t sum = 0; uint16_t hi = 0;
      for (unsigned i = 0; i < 64; ++i) { uint16_t v = analogRead(DET_PIN); sum += v; if (v > hi) hi = v; delayMicroseconds(300); }
      Serial.printf("R,%.0f,%u\n", sum / 64.0f, hi);
      reportAt = millis();
    }
    delay(1);
    return;
  }
  if (live) {
    if (millis() - holdAt >= LIVE_HOLD_MS) { stopTest(); Serial.println("# AUTO OFF after 5 min"); }
    else liveSweep();
    delay(1);
    return;
  }
  if (scanning && millis() - phaseAt >= SETTLE_MS) {
    if (phase == 0) {
      baseline = readDetector();
      if (!tune(hz)) return;
      phase = 1; phaseAt = millis();
    } else {
      if (!clockHealthy()) { stopTest(); Serial.println("# ERROR: clock lost; scan invalid, OFF requested"); return; }
      auto r = readDetector();
      float delta = r.mean - baseline.mean;
      anyClip |= r.high >= 4000 || baseline.high >= 4000;
      Serial.printf("%lu,%.1f,%.1f,%.1f,%u,%u,%u,%u\n", (unsigned long)hz,
                    baseline.mean, r.mean, delta, r.low, r.high, baseline.low, baseline.high);
      if (delta > bestDelta) { bestDelta = delta; bestHz = hz; }
      outputsOff();
      if (hz == lastHz) {
        scanning = false;
        Serial.printf("# DONE RF OFF. Largest sample: %lu Hz, delta=%.1f; NOT automatic resonance confirmation.\n", (unsigned long)bestHz, bestDelta);
        if (bestHz == firstHz || bestHz == lastHz) Serial.println("# EDGE maximum: widen/retune and repeat.");
        if (bestDelta < 100) Serial.println("# WEAK response: ADC bottom range/diode threshold; result inconclusive.");
        if (anyClip) Serial.println("# CLIPPED: result invalid; check detector DC with multimeter.");
      } else { hz += stepHz; phase = 0; phaseAt = millis(); }
    }
  } else if (!scanning && rfOn) {
    if (millis() - holdAt >= HOLD_MS) { stopTest(); Serial.println("# AUTO OFF after 60 s"); }
    else if (millis() - reportAt >= 250) {
      if (!clockHealthy()) { stopTest(); Serial.println("# ERROR: clock lost; OFF requested"); return; }
      auto r = readDetector();
      Serial.printf("raw:%.1f\n", r.mean);
      reportAt = millis();
    }
  }
  delay(1);
}
