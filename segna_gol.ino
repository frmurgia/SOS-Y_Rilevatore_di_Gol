#include <FastLED.h>

// ====== LED ======
#define LED_PIN     6
#define NUM_LEDS    100
#define BRIGHTNESS  180
CRGB leds[NUM_LEDS];

// ====== SENSORI ======
const int TRIG_TOP = A1, ECHO_TOP = A0;
const int TRIG_LAT = A2, ECHO_LAT = A3;

// ====== CALIBRAZIONE ======
float baseTop, baseLat;
const float SOGLIA_TOP = 0.92;
const float SOGLIA_LAT = 0.70;

// ====== TIMING ======
unsigned long ultimoGol = 0;
const unsigned long COOLDOWN = 3000;

uint8_t goals = 0;

// ───────────────────────────────────────────────
//  SENSORE — timeout per 200cm+
// ───────────────────────────────────────────────
float misura(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long d = pulseIn(echo, HIGH, 15000);
  if (d == 0) return 0;
  return d * 0.0343 / 2.0;
}

// ───────────────────────────────────────────────
//  CALIBRAZIONE
// ───────────────────────────────────────────────
void calibra() {
  Serial.println("Calibrazione... porta libera!");
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.setBrightness(80);
  FastLED.show();

  float sumTop = 0, sumLat = 0;
  int countTop = 0, countLat = 0;
  for (int i = 0; i < 20; i++) {
    float t = misura(TRIG_TOP, ECHO_TOP);
    if (t > 0) { sumTop += t; countTop++; }
    delay(30);
    float l = misura(TRIG_LAT, ECHO_LAT);
    if (l > 0) { sumLat += l; countLat++; }
    delay(30);
  }
  baseTop = (countTop > 0) ? sumTop / countTop : 140;
  baseLat = (countLat > 0) ? sumLat / countLat : 200;

  Serial.print("Base traversa: "); Serial.print(baseTop, 1); Serial.println(" cm");
  Serial.print("Base palo: ");     Serial.print(baseLat, 1); Serial.println(" cm");

  FastLED.clear(true);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.println("Pronto! Si gioca!");
}

// ───────────────────────────────────────────────
//  IDLE — SEGNAPUNTI
//  5 LED rossi per gol, 5 spenti di separazione
// ───────────────────────────────────────────────
void mostraGol() {
  FastLED.clear();
  if (goals == 0) {
    for (int i = 0; i < 5; i++) leds[i] = CRGB::Green;
  } else {
    for (int g = 0; g < goals && g < 10; g++) {
      int start = g * 10;
      for (int i = 0; i < 5; i++) {
        if (start + i < NUM_LEDS) leds[start + i] = CRGB::Red;
      }
    }
  }
  FastLED.show();
}

// ───────────────────────────────────────────────
//  VITTORIA — 10 GOL
// ───────────────────────────────────────────────
void celebraVittoria() {
  Serial.println("10 GOL! VITTORIA!");

  int center = NUM_LEDS / 2;
  for (int r = 0; r <= center; r += 2) {
    fadeToBlackBy(leds, NUM_LEDS, 80);
    int left  = center - r;
    int right = center + r;
    if (left  >= 0)       leds[left]  = CRGB::Green;
    if (right < NUM_LEDS) leds[right] = CRGB::Green;
    FastLED.show();
    delay(12);
  }

  const CRGB palette[] = {
    CRGB::Green, CRGB::White, CRGB(0, 255, 100), CRGB(100, 255, 0)
  };
  uint32_t start = millis();
  while (millis() - start < 3000) {
    fadeToBlackBy(leds, NUM_LEDS, 100);
    for (int s = 0; s < 7; s++) {
      leds[random16(NUM_LEDS)] = palette[random8(4)];
    }
    FastLED.show();
    delay(25);
  }

  for (int i = 0; i < 3; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show(); delay(200);
    FastLED.clear(); FastLED.show(); delay(150);
  }

  fill_solid(leds, NUM_LEDS, CRGB::Green);
  for (int b = BRIGHTNESS; b >= 0; b -= 4) {
    FastLED.setBrightness(b);
    FastLED.show();
    delay(8);
  }
  FastLED.clear(true);
  FastLED.setBrightness(BRIGHTNESS);

  goals = 0;
  Serial.println("Reset! Nuova partita.");
}

// ───────────────────────────────────────────────
//  ANIMAZIONE GOL
// ───────────────────────────────────────────────
void phase_flash() {
  for (int i = 0; i < 2; i++) {
    FastLED.setBrightness(255);
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show(); delay(50);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show(); delay(30);
  }
  FastLED.setBrightness(BRIGHTNESS);
}

void phase_redWave() {
  int center = NUM_LEDS / 2;
  for (int radius = 0; radius <= center; radius += 2) {
    fadeToBlackBy(leds, NUM_LEDS, 80);
    int left  = center - radius;
    int right = center + radius;
    if (left  >= 0)       leds[left]  = CRGB::Red;
    if (right < NUM_LEDS) leds[right] = CRGB::Red;
    FastLED.show();
    delay(12);
  }
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
  delay(200);
}

void phase_sparkle() {
  const CRGB palette[] = {
    CRGB::Red, CRGB::White, CRGB(255, 180, 0), CRGB(255, 40, 0)
  };
  uint32_t start = millis();
  while (millis() - start < 2000) {
    fadeToBlackBy(leds, NUM_LEDS, 100);
    for (int s = 0; s < 7; s++) {
      leds[random16(NUM_LEDS)] = palette[random8(4)];
    }
    FastLED.show();
    delay(25);
  }
}

void phase_fadeOut() {
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  for (int b = BRIGHTNESS; b >= 0; b -= 4) {
    FastLED.setBrightness(b);
    FastLED.show();
    delay(8);
  }
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
}

void celebraGol() {
  goals++;
  Serial.print("GOL! Totale: ");
  Serial.println(goals);

  phase_flash();
  phase_redWave();
  phase_sparkle();
  phase_fadeOut();

  if (goals >= 10) {
    celebraVittoria();
  }
}

// ───────────────────────────────────────────────
//  SETUP
// ───────────────────────────────────────────────
void setup() {
  pinMode(TRIG_TOP, OUTPUT); pinMode(ECHO_TOP, INPUT);
  pinMode(TRIG_LAT, OUTPUT); pinMode(ECHO_LAT, INPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);

  random16_set_seed(analogRead(A4));

  Serial.begin(9600);
  delay(1000);
  calibra();
}

// ───────────────────────────────────────────────
//  LOOP (~20ms per ciclo = ~50 letture/sec)
// ───────────────────────────────────────────────
void loop() {
  float top = misura(TRIG_TOP, ECHO_TOP);
  delayMicroseconds(500);
  float lat = misura(TRIG_LAT, ECHO_LAT);

  bool trigTop = (top > 0 && top < baseTop * SOGLIA_TOP);
  bool trigLat = (lat > 0 && lat < baseLat * SOGLIA_LAT);

  bool gol = trigTop || trigLat;

  unsigned long ora = millis();
  if (gol && (ora - ultimoGol > COOLDOWN)) {
    ultimoGol = ora;

    Serial.print("Trigger: ");
    if (trigTop) Serial.print("TRAVERSA ");
    if (trigLat) Serial.print("PALO ");
    Serial.print("| Top="); Serial.print(top, 1);
    Serial.print(" Lat="); Serial.println(lat, 1);

    celebraGol();
  } else {
    mostraGol();
  }
}
