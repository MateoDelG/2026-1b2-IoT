#include "neopixelManagerNamespace.h"

// Estas constantes definen la configuración fija de la cinta.
// Si quieres cambiar de pin o cantidad de píxeles,
// lo haces aquí.
#define NEOPIXEL_PIN 14
#define NUM_PIXELS 4

namespace neopixelManagerNamespace {

  // Este objeto controla la cinta WS2812B.
  // Se crea una sola vez con valores fijos.
  // Así evitamos punteros y evitamos usar ->
  Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

  void begin() {
    strip.begin();
    // Inicializa la comunicación con la cinta

    strip.show();
    // Apaga todos los LEDs al iniciar
  }

  void turnOff() {
    strip.clear();
    // Borra todos los colores almacenados

    strip.show();
    // Envía el cambio a la cinta
  }

  void showColor(uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
      // Asigna el mismo color a todos los píxeles
    }

    strip.show();
    // Muestra el cambio en la cinta
  }

  void showPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < NUM_PIXELS) {
      strip.setPixelColor(index, strip.Color(r, g, b));
      // Cambia el color de un solo píxel

      strip.show();
      // Envía el cambio a la cinta
    }
  }

  void blink(uint8_t r, uint8_t g, uint8_t b, int delayMs) {
    showColor(r, g, b);
    // Enciende toda la cinta

    delay(delayMs);
    // Espera el tiempo indicado

    turnOff();
    // Apaga toda la cinta

    delay(delayMs);
    // Espera nuevamente
  }

}