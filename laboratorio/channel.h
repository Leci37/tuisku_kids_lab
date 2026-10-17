// 📻 El canal de la wifi de la placa: el más libre.
//
// Antes de crear su wifi, la placa escucha las redes que hay alrededor y elige,
// de los tres canales que no se pisan (1, 6 y 11), el que tiene menos ruido.
// Cada red cuenta más cuanto más fuerte se oye y cuanto más cerca está su
// canal: una red ocupa unos cuatro canales a cada lado del suyo, así que una
// en el 3 molesta al 1 y también al 6. Si hay empate (una sala sin redes, o
// varias placas que arrancan a la vez y oyen lo mismo), decide el número de
// serie de cada placa: así no se van todas al mismo canal.
//
// Va en su pestaña y sin nada de Arduino para poder probarlo en el ordenador.
#pragma once

#include <stddef.h>
#include <stdint.h>

const uint8_t QUIET_CHANNELS[] = {1, 6, 11};
const uint8_t QUIET_CHANNEL_COUNT = sizeof(QUIET_CHANNELS) / sizeof(QUIET_CHANNELS[0]);

// Lo que estorban a un canal las redes oídas (su canal y su fuerza, en dBm).
inline uint32_t channelLoad(uint8_t channel, const uint8_t* channels, const int8_t* rssi, size_t count) {
  uint32_t load = 0;
  for (size_t i = 0; i < count; i++) {
    if (channels[i] < 1 || channels[i] > 14) continue;
    int distance = channels[i] > channel ? channels[i] - channel : channel - channels[i];
    if (distance > 4) continue;
    // De -100 dBm (casi nada) a -30 dBm (pegada a la placa): de 1 a 70.
    int strength = rssi[i] + 100;
    if (strength < 1) strength = 1;
    if (strength > 70) strength = 70;
    // En su mismo canal pesa 5 veces; a 4 canales, 1 vez.
    load += (uint32_t)strength * (uint32_t)(5 - distance);
  }
  return load;
}

// El canal más libre de los tres; si empatan, el que diga el número de serie.
inline uint8_t quietestChannel(const uint8_t* channels, const int8_t* rssi, size_t count, uint32_t serial) {
  uint32_t loads[QUIET_CHANNEL_COUNT];
  uint32_t least = 0xFFFFFFFFu;
  for (uint8_t i = 0; i < QUIET_CHANNEL_COUNT; i++) {
    loads[i] = channelLoad(QUIET_CHANNELS[i], channels, rssi, count);
    if (loads[i] < least) least = loads[i];
  }
  uint8_t tied = 0;
  for (uint8_t i = 0; i < QUIET_CHANNEL_COUNT; i++) {
    if (loads[i] == least) tied++;
  }
  uint8_t pick = serial % tied;
  for (uint8_t i = 0; i < QUIET_CHANNEL_COUNT; i++) {
    if (loads[i] != least) continue;
    if (pick == 0) return QUIET_CHANNELS[i];
    pick--;
  }
  return QUIET_CHANNELS[0];  // no se llega aquí
}
