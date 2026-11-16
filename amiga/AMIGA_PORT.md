# Netacka for GFOS

Netacka by Paweł Marczewski, with bots by Michał Dereziński and Michał
Gawroński (MIT), built for AmigaOS 3.2 / GFOS on a 68040, 68060 or
68080. No FPU is needed.

Each player steers a line that never stops growing. Touch a line or the
wall and you are out of the round; the last one left scores. Play
against the bots on one Amiga, or with friends over the network.

## What is in the drawer

- `Netacka`: the game.
- `netacka.ini`: the settings (window, server address, port, screen size,
  speed, score limit). Updates keep yours.
- `Libs/allegro4.library` and `Libs/acgame.library`: Allegro 4 for GFOS
  (gfopenallegro) on ACGame. The game opens them from its own `Libs/`.

## Playing

Netacka always plays through a server, even against bots on one Amiga,
so a TCP/IP stack (Roadshow) has to be running. In the menu, start a
server and add bots, or join a server by its address. The default port
is 6789.

## How the port is made

- The game and libnet (vendored in `libnet/`) are upstream's, built with
  68040 soft float against gfopenallegro's Allegro 4
  (`amiga/Makefile.amigaos`).
- libnet is built with no `TARGET_*`, so it uses BSD sockets; its
  internet driver's socket calls go to bsdsocket.library through
  `amiga/amiga_net.h`, which only those two files include.
- libnix's start-up code is called `start`, like the game's menu, so the
  game's is built as `netacka_start`.
- allegro4.library draws in 8 bits through a palette. Netacka's colours
  all come from its palette, so on GFOS it skips upstream's 24/32-bit
  check.
- Without a TCP/IP stack the game says so and stops, instead of quitting
  without a word.

## Tests

`tests/amiga/test_host.sh` builds libnet the way the Amiga build does and
sends the game's packets through it (BSD sockets over the loopback and
the local driver), then reads `netacka.ini` through gfopenallegro's own
config code. `package/build-amiga.sh` runs it before each build.
