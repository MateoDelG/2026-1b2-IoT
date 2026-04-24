#ifndef NEOPIXELMANAGERNAMESPACE_H
#define NEOPIXELMANAGERNAMESPACE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

namespace neopixelManagerNamespace {

  void begin();
  // Inicializa la cinta

  void turnOff();
  // Apaga todos los píxeles

  void showColor(uint8_t r, uint8_t g, uint8_t b);
  // Enciende todos los píxeles con el mismo color

  void showPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
  // Enciende un solo píxel

  void blink(uint8_t r, uint8_t g, uint8_t b, int delayMs);
  // Hace parpadear toda la cinta
}

#endif