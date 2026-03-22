# ⚽ Segna-Gol — Rilevatore di Gol con Arduino

Sistema automatico di rilevamento gol per porte da calcetto, con animazioni LED e segnapunti visivo.

![Arduino](https://img.shields.io/badge/Arduino-UNO-00979D?logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green)

![Segna-Gol](images/foto_progetto.jpg)

---

## Descrizione

Segna-Gol utilizza due sensori ultrasonici HC-SR04 posizionati sulla struttura della porta per rilevare il passaggio del pallone. Quando viene segnato un gol, una striscia di 100 LED WS2812B si anima con una sequenza celebrativa. Un segnapunti visivo tiene traccia dei gol segnati, e al raggiungimento di 10 gol viene attivata un'animazione di vittoria.

---

## Caratteristiche

- **Rilevamento a doppio sensore** — traversa + palo laterale con logica OR per massima sensibilità
- **Calibrazione automatica** — il sistema si calibra all'accensione leggendo le distanze a porta vuota
- **Segnapunti LED** — ogni gol accende un blocco di 5 LED rossi, separati da 5 LED spenti
- **Animazione gol** — sequenza in 4 fasi: flash bianco, onda rossa, coriandoli, fade out
- **Animazione vittoria** — al 10° gol, celebrazione verde con onda, coriandoli e flash
- **Reset automatico** — dopo la vittoria il contatore si azzera per una nuova partita
- **Lettura veloce** — ciclo di ~20ms (~50 letture/sec) per intercettare anche tiri potenti
- **Anti-rimbalzo** — cooldown di 3 secondi tra un gol e l'altro per evitare doppie letture

---

## Hardware

### Componenti

| Componente | Quantità | Note |
|---|---|---|
| Arduino UNO | 1 | |
| HC-SR04 | 2 | Sensori ultrasonici |
| Striscia WS2812B | 1 | 100 LED |
| Alimentatore 5V | 1 | ≥3A per 100 LED a piena luminosità |

### Schema collegamenti
```
SENSORE TRAVERSA (sopra, puntato verso il basso)
├── VCC  → 5V
├── GND  → GND
├── TRIG → A1
└── ECHO → A0

SENSORE PALO (laterale, puntato orizzontalmente)
├── VCC  → 5V
├── GND  → GND
├── TRIG → A2
└── ECHO → A3

STRISCIA LED WS2812B
├── 5V   → Alimentatore 5V (NON da Arduino)
├── GND  → GND comune con Arduino
└── DIN  → Pin 6 (tramite resistenza 470Ω)
```

### Posizionamento sensori
```
          ┌──── SENSORE TRAVERSA ────┐
          │      (A1/A0)             │
          │        ↓↓↓               │
          │                          │
  200 cm  │                          │
          │                          │
          │                          │  140 cm
  SENSORE │→→→→→→→→→→→→→→→→→→→→→→→→→│
  PALO    │                          │
  (A2/A3) │                          │
          │                          │
          └──────────────────────────┘
                   TERRENO
```

- **Traversa**: montato al centro della traversa, leggermente angolato verso il basso (~10°)
- **Palo**: montato a metà altezza (~70cm), puntato orizzontalmente verso il palo opposto

---

## Software

### Dipendenze

- [FastLED](https://github.com/FastLED/FastLED) — gestione striscia LED WS2812B

Installabile dal Library Manager di Arduino IDE.

### Parametri configurabili
```cpp
#define NUM_LEDS    100       // Numero LED sulla striscia
#define BRIGHTNESS  180       // Luminosità (0-255)

const float SOGLIA_TOP = 0.92;  // Sensibilità traversa (0.80-0.95)
const float SOGLIA_LAT = 0.70;  // Sensibilità palo (0.60-0.80)

const unsigned long COOLDOWN = 3000;  // Pausa tra gol (ms)
```

### Logica di rilevamento

Il sistema rileva un gol quando **almeno uno** dei due sensori registra una distanza significativamente inferiore alla baseline calibrata:

- **Sensore palo**: scatta se la distanza scende sotto il 70% della baseline
- **Sensore traversa**: scatta se la distanza scende sotto il 92% della baseline (soglia più alta perché il pallone a terra è lontano dal sensore)

### Segnapunti visivo
```
0 gol:  🟢🟢🟢🟢🟢 ░░░░░░░░░░░░░░░ ...
1 gol:  🔴🔴🔴🔴🔴 ░░░░░ ░░░░░░░░░ ...
2 gol:  🔴🔴🔴🔴🔴 ░░░░░ 🔴🔴🔴🔴🔴 ░░░░░ ...
...
10 gol: 🏆 ANIMAZIONE VITTORIA → RESET
```

Ogni gol = 5 LED rossi accesi, seguiti da 5 LED spenti di separazione.

---

## Installazione

1. Collegare i componenti secondo lo schema
2. Installare la libreria **FastLED** dal Library Manager
3. Caricare lo sketch su Arduino UNO
4. All'accensione, lasciare la porta libera per la calibrazione (~2 sec, LED verdi)
5. Quando i LED si spengono e appaiono 5 LED verdi → sistema pronto

---

## Troubleshooting

| Problema | Causa probabile | Soluzione |
|---|---|---|
| Non rileva i gol | Soglia troppo bassa | Alzare `SOGLIA_TOP` / `SOGLIA_LAT` |
| Falsi positivi | Soglia troppo alta o riflessi | Abbassare le soglie, controllare superfici riflettenti |
| Lettura 0 su un sensore | Cavo scollegato o pin errato | Verificare TRIG/ECHO, eseguire sketch di test singolo |
| LED non si accendono | Alimentazione insufficiente | Usare alimentatore esterno 5V ≥3A |
| Gol doppi | Cooldown troppo corto | Aumentare `COOLDOWN` |

### Debug

Aprire il Serial Monitor a **9600 baud** per visualizzare:

- Valori di calibrazione (distanze baseline)
- Quale sensore ha triggerato il gol
- Distanze in tempo reale

---

## Possibili miglioramenti

- Aggiunta di un buzzer per feedback sonoro
- Display OLED per il conteggio gol
- Modalità due squadre con due porte
- Salvataggio punteggio su EEPROM
- Controllo wireless via ESP32 / Bluetooth

---

## Licenza

MIT — Usa, modifica e condividi liberamente.
