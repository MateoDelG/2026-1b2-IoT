#ifndef NEOPIXELMANAGERCLASS_H
#define NEOPIXELMANAGERCLASS_H
// Estas líneas evitan que este archivo se cargue más de una vez.
// Si el compilador intentara incluirlo repetidamente, podrían aparecer errores.
// Es una protección común en archivos .h.

#include <Arduino.h>
// Incluye definiciones básicas de Arduino.
// Gracias a esto podemos usar tipos como uint8_t y funciones comunes del entorno Arduino.

#include <Adafruit_NeoPixel.h>
// Incluye la librería que realmente sabe cómo controlar una cinta WS2812B.
// Nuestra clase no "reemplaza" esta librería: más bien la envuelve para usarla
// de forma más organizada y didáctica.

class neopixelManagerClass {
// Aquí estamos declarando una CLASE.
//
// Una clase puede explicarse como una "plantilla" o "molde" para crear objetos.
// Un objeto es una variable más compleja que no solo guarda datos,
// sino que también tiene funciones asociadas.
//
// En este caso, la clase neopixelManagerClass representa un "administrador"
// de una cinta NeoPixel.
//
// La idea pedagógica es esta:
// - la clase agrupa datos relacionados con la cinta
// - y también agrupa funciones para manipular esa cinta
//
// En otras palabras:
// esta clase junta en un mismo lugar:
// 1. la configuración de la cinta
// 2. el objeto que la controla
// 3. las funciones para encender, apagar y mostrar colores

  private:
  // private significa "privado".
  //
  // Todo lo que se ponga aquí solo puede ser usado desde dentro de la clase.
  // El programa principal NO puede acceder directamente a estas variables.
  //
  // Esta es una idea importante de las clases:
  // proteger los datos internos y obligar a que se usen a través de métodos.
  //
  // Para estudiantes principiantes, esto se puede explicar así:
  // "private son detalles internos del funcionamiento de la clase".

    uint8_t pin;
    // Este atributo guarda el número del pin del ESP32
    // donde está conectada la línea de datos de la cinta WS2812B.
    // Atributo = variable que pertenece a la clase.

    uint8_t numPixels;
    // Este atributo guarda la cantidad de LEDs o píxeles de la cinta.

    Adafruit_NeoPixel strip;
    // Este atributo es otro objeto.
    //
    // Es importante entender esto:
    // nuestra clase contiene internamente un objeto de la librería Adafruit_NeoPixel.
    //
    // Es decir:
    // - la librería Adafruit_NeoPixel es la que sabe hablar realmente con la cinta
    // - nuestra clase organiza ese uso de forma más simple
    // Entonces, strip es el "motor real" que controla la WS2812B.

  public:
  // Todo lo que se ponga aquí sí puede ser usado desde fuera de la clase,
  // por ejemplo desde main.
  // "public es la parte visible de la clase, lo que el usuario sí puede usar".
  //
  // En este bloque normalmente se colocan:
  // - constructores
  // - métodos que queremos que el usuario llame

    neopixelManagerClass(uint8_t stripPin, uint8_t pixelCount = 4);
    // Este es el CONSTRUCTOR de la clase.
    //
    // El constructor es una función especial que se ejecuta automáticamente
    // cuando se crea un objeto.
    //
    // Por ejemplo, cuando en main escribimos:
    // neopixelManagerClass strip(5, 4);
    //
    // en ese momento se llama automáticamente a este constructor.
    //
    // ¿Qué recibe?
    // - stripPin: el pin donde está conectada la cinta
    // - pixelCount: cuántos píxeles tiene la cinta
    //
    // El = 4 significa que, si el usuario no especifica la cantidad,
    // por defecto se asumirá que la cinta tiene 4 píxeles.
    //
    // Esto es útil en tu caso porque quieres enseñar con una tira corta de 4 LEDs.

    void begin();
    // Método para inicializar la cinta.
    //
    // Un método es simplemente una función que pertenece a una clase.
    //
    // La idea de begin() es parecida a setup() para este objeto:
    // prepara la cinta para empezar a usarse.

    void turnOff();
    // Método para apagar todos los píxeles.
    //
    // En vez de escribir toda la lógica de apagar la cinta en main.ino,
    // simplemente llamamos a este método y la clase se encarga del detalle.

    void showColor(uint8_t r, uint8_t g, uint8_t b);
    // Método para poner todos los LEDs del mismo color.
    //
    // Recibe tres valores:
    // - r = intensidad de rojo
    // - g = intensidad de verde
    // - b = intensidad de azul
    //
    // Cada valor normalmente va de 0 a 255.

    void showPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    // Método para cambiar el color de un único píxel.
    //
    // index indica cuál LED queremos modificar.
    // Luego se indican los valores de rojo, verde y azul para ese LED.

    void blink(uint8_t r, uint8_t g, uint8_t b, int delayMs);
    // Método para hacer parpadear toda la cinta.
    //
    // Recibe:
    // - el color con el que parpadeará
    // - el tiempo de espera en milisegundos
};

#endif
// Fin de la protección del archivo de encabezado.