# POV355 — Persistence-of-Vision LED Clock

An **ENGR 355 (Winter 2025)** embedded-systems project: a battery-style
persistence-of-vision clock that spins a ring of **16 LEDs** to display the
current time and ambient temperature, with automatic brightness from a light
sensor and time-over-UART telemetry.

**Target board:** NXP **FRDM-KL16Z4** (Kinetis MKL16Z128xxx4, Cortex-M0+)
**Toolchain:** Keil µVision (MDK-ARM), ARM Compiler 5, C++

AI was used to generate the readme. No code was generated with AI

---

## How it works

A POV display fakes a still image by scanning one LED at a time at high speed
and letting persistence-of-vision fill in the gaps. This project shifts a
single-column bitmap through the ring on every **PIT interrupt**, refreshing the
whole frame thousands of times a second.

| Driver | Role |
| --- | --- |
| `DLED` | Renders `HH:MM` + `°F` from a bitmap font into a 184-slot buffer and shifts one slot per PIT IRQ onto the 16 LEDs. Also owns the TPM1 PWM brightness channel. |
| `DADC` | ADC0 driver. Converts a **thermistor** and an **ambient-light sensor** to temperature and light level. Includes a 120-entry thermistor lookup table. |
| `DRTC` | Wraps the RTC seconds counter (`RTC->TSR`) and applies hour/minute user offsets from the two set buttons. |
| `DUART` | UART0 driver — streams `Time: HH:MM:SS` and echoes received text uppercased. |
| `interrupts.cpp` | C-linkage IRQ stubs (`PIT`, `PORTC_PORTD`, `UART0`) that dispatch into the C++ drivers. |

Rendering pipeline:

```
DRTC.GetSeconds()  ─┐
DADC.GetTemperature() ──> DLED.convert() ──> DLED.updateTime() ──> currentDisplay[184]
DADC.GetAmbientLight() ──> DLED.SetBrightness() (TPM1 PWM duty)      │
                                                                       v
                                            PIT IRQ @ ~22 kHz ──> DLED.clock()
                                                                       │
                                                    UpdatePOV(currentDisplay[i])
                                                                       v
                                                        PTC[2..11] + PTD[2..7]
```

## Pin map

| Pin | Function | Notes |
| --- | --- | --- |
| `PTC2`–`PTC11` | LEDs 0–9 | GPIO output, MUX 1 |
| `PTD2`–`PTD7` | LEDs 10–15 | GPIO output, MUX 1 |
| `PTB0` | LED brightness PWM | `TPM1_CH0`, MUX 3 |
| `PTB2` | Thermistor | `ADC0_CH12` |
| `PTB3` | Ambient light sensor | `ADC0_CH13` |
| `PTA1` | UART0 TX | MUX 2, 9600 baud |
| `PTA2` | UART0 RX | MUX 2 |
| `PTE20` | Minute-set button | internal pull-up, **active low** |
| `PTE21` | Hour-set button | internal pull-up, **active low** |
| `PORTC1` | 32.768 kHz RTC crystal | `SIM_SOPT1_OSC32KSEL(2)` |
| `PTD1` | GPIO IRQ source | IRQC 0b1011 — *see Known Issues* |

