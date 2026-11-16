/* Netacka for GFOS: start-up bits. libnix's start-up code is called
 * start, like the game's own start() (its menu), so the game's is built
 * as netacka_start (amiga/Makefile.amigaos).
 * Copyright (c) 2026 AmiCradle Team. MIT, as Netacka. */
unsigned long __stack = 256 * 1024;
static const char amiga_ver[] __attribute__((used)) =
  "$VER: Netacka 1.0 (16.11.2026)";
