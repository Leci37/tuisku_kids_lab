// ⚙️ Configuración del Laboratorio de inventos.
// Lo que se puede cambiar sin miedo está aquí: el nombre de la wifi y los pines.
#pragma once

#include <Arduino.h>

#define LAB_VERSION "1.2.2"

// --- La wifi -----------------------------------------------------------------
// La placa crea su propia wifi, «Laboratorio-XXXX». Las cuatro últimas letras
// salen del número de serie de cada placa: en una clase con varias, no se mezclan.
#define WIFI_AP_PREFIX "Laboratorio-"

// Contraseña de esa wifi. Vacía = wifi abierta. Si pones una, de 8 letras o más.
#define WIFI_AP_PASSWORD ""

// El canal de esa wifi. 0 = el más libre: al arrancar, la placa mira qué redes
// hay alrededor y elige el 1, el 6 o el 11 (channel.h). O un número del 1 al 13
// para dejarlo fijo.
#define WIFI_AP_CHANNEL 0

// Opcional: para usarla con la wifi de casa, escribe aquí su nombre y su
// contraseña. Si en 15 segundos no consigue entrar, crea su propia wifi.
#define WIFI_HOME_SSID ""
#define WIFI_HOME_PASSWORD ""

// La dirección con nombre: http://laboratorio.local
#define LAB_HOSTNAME "laboratorio"

// --- Los pines ---------------------------------------------------------------
// Los nombres D1, D5… son los de la NodeMCU y la Wemos D1 mini, que los tienen
// en los mismos pines. Otras placas no los tienen (la genérica, la ESP8285) o
// los ponen en otro sitio (la Wemos D1 R1: su D1 es el TX y su D8, un pin de
// arranque): con ellas, un error claro en vez de muchos raros o de pines mal.
// Los nombres cambian con el núcleo: ESP8266_NODEMCU es el de la 2.7.4.
#if !defined(ARDUINO_ESP8266_NODEMCU_ESP12E) && !defined(ARDUINO_ESP8266_NODEMCU_ESP12) && \
    !defined(ARDUINO_ESP8266_NODEMCU) && !defined(ARDUINO_ESP8266_WEMOS_D1MINI) && \
    !defined(ARDUINO_ESP8266_WEMOS_D1MINILITE) && !defined(ARDUINO_ESP8266_WEMOS_D1MINIPRO)
#error "Placa equivocada: en Herramientas > Placa elige «NodeMCU 1.0 (ESP-12E Module)» (o «LOLIN(WEMOS) D1 R2 & mini» si es una D1 mini)"
#endif

// 🚦 Las tres luces del semáforo (cada una con su resistencia de 220 Ω).
#define PIN_RED D5
#define PIN_YELLOW D6
#define PIN_GREEN D7

// 🔊 El zumbador (opcional): suena en el detector de fantasmas y en los bloques.
#define PIN_BUZZER D8

// 🎤 El micrófono: su pata de señal (OUT o AO) va a A0, la única analógica.
#define PIN_MIC A0

// 👻 El sensor de distancia por ultrasonidos (HC-SR04 o HC-SR04P).
#define PIN_TRIG D1
#define PIN_ECHO D2

// 🔘 El botón FLASH que ya trae la placa: es el botón de los peatones.
#define PIN_BUTTON D3

// 💙 La lucecita azul de la placa: parpadea para decir «estoy viva».
#define PIN_STATUS_LED LED_BUILTIN
