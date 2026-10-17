"""El canal de la wifi de la placa (laboratorio/channel.h): el más libre de 1, 6 y 11.

channel.h no usa nada de Arduino: se compila en el ordenador y se le pasan las
redes que «oiría» la placa (su canal y su fuerza en dBm).
"""
from __future__ import annotations

import shutil
import subprocess
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parent.parent
FIRMWARE = ROOT / "laboratorio"

HARNESS = r"""
#include <stdio.h>
#include "channel.h"

// Una línea por caso: el número de serie y luego las redes, «canal fuerza».
// Contesta el canal elegido.
int main() {
  static char line[4096];
  while (fgets(line, sizeof(line), stdin)) {
    uint8_t channels[64];
    int8_t rssi[64];
    size_t count = 0;
    unsigned long serial;
    int used;
    char* p = line;
    if (sscanf(p, "%lu%n", &serial, &used) != 1) return 1;
    p += used;
    int channel, strength;
    while (count < 64 && sscanf(p, "%d %d%n", &channel, &strength, &used) == 2) {
      channels[count] = (uint8_t)channel;
      rssi[count] = (int8_t)strength;
      count++;
      p += used;
    }
    printf("%u\n", quietestChannel(channels, rssi, count, (uint32_t)serial));
  }
  return 0;
}
"""


@pytest.fixture(scope="module")
def choose(tmp_path_factory):
    compiler = shutil.which("g++") or shutil.which("clang++")
    if compiler is None:
        pytest.skip("sin compilador de C++ (en el CI siempre hay)")
    folder = tmp_path_factory.mktemp("canal")
    (folder / "harness.cpp").write_text(HARNESS, encoding="utf-8")
    program = folder / "harness"
    subprocess.run(
        [compiler, "-std=c++11", "-Wall", "-Wextra", "-Werror", "-I", str(FIRMWARE), str(folder / "harness.cpp"), "-o", str(program)],
        check=True,
    )

    def run(networks: list[tuple[int, int]], serial: int = 0) -> int:
        line = " ".join([str(serial)] + [f"{channel} {rssi}" for channel, rssi in networks])
        out = subprocess.run([str(program)], input=line + "\n", capture_output=True, text=True, check=True)
        return int(out.stdout)

    return run


def test_in_a_quiet_room_each_board_takes_a_different_channel(choose):
    # Sin redes, empatan los tres: decide el número de serie, y tres placas
    # que arrancan a la vez no se van todas al mismo.
    assert [choose([], serial) for serial in (0, 1, 2, 3)] == [1, 6, 11, 1]


def test_it_avoids_the_channel_of_a_network_next_to_it(choose):
    for serial in range(4):
        assert choose([(1, -45)], serial) in (6, 11)
        assert choose([(6, -45)], serial) in (1, 11)
        assert choose([(1, -45), (6, -60)], serial) == 11


def test_a_network_in_between_bothers_both_neighbours(choose):
    # Una red en el 3 se come el 1 y el 6; en el 9, el 6 y el 11.
    assert choose([(3, -50)]) == 11
    assert choose([(9, -50)]) == 1


def test_a_strong_network_bothers_more_than_several_weak_ones(choose):
    # Las tres débiles del 11 molestan menos que la fuerte de cada uno de los otros.
    weak = [(11, -90)] * 3
    assert choose([(1, -40), (6, -40)] + weak) == 11
    # …pero muchas débiles acaban pesando más que una no tan fuerte.
    assert choose([(1, -80), (6, -40)] + [(11, -90)] * 6) == 1


def test_if_all_are_busy_it_takes_the_least_busy(choose):
    assert choose([(1, -40), (1, -50), (6, -70), (11, -40), (11, -60)]) == 6


def test_it_ignores_channels_that_do_not_exist(choose):
    assert choose([(0, -30), (15, -30), (200, -30)], serial=1) == 6
