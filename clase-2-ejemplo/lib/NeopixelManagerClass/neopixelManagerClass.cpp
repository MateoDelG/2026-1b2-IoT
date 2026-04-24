#include "neopixelManagerClass.h"

neopixelManagerClass::neopixelManagerClass(uint8_t stripPin, uint8_t pixelCount)
  : pin(stripPin),
    numPixels(pixelCount),
    strip(pixelCount, stripPin, NEO_GRB + NEO_KHZ800)
{
  // NEO_GRB y NEO_KHZ800 son configuraciones típicas de la WS2812B
}

void neopixelManagerClass::begin() {
  strip.begin();
  // Inicializa la comunicación con la cinta

  strip.show();
  // Deja todos los LEDs apagados al iniciar
}

void neopixelManagerClass::turnOff() {
  strip.clear();
  // Borra todos los colores guardados

  strip.show();
  // Envía el cambio a la cinta
}

void neopixelManagerClass::showColor(uint8_t r, uint8_t g, uint8_t b) {
  for (uint8_t i = 0; i < numPixels; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
    // Asigna el mismo color a todos los píxeles
  }

  strip.show();
  // Muestra el cambio
}

void neopixelManagerClass::showPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index < numPixels) {
    strip.setPixelColor(index, strip.Color(r, g, b));
    // Cambia el color de un solo píxel

    strip.show();
    // Envía el cambio a la cinta
  }
}

void neopixelManagerClass::blink(uint8_t r, uint8_t g, uint8_t b, int delayMs) {
  showColor(r, g, b);
  // Enciende toda la cinta

  delay(delayMs);
  // Espera

  turnOff();
  // Apaga toda la cinta

  delay(delayMs);
  // Espera nuevamente
}