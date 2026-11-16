#!/bin/sh
# Netacka for GFOS, host tests: libnet as the Amiga build configures it
# carries the game's packets, and netacka.ini reads back through
# gfopenallegro's own config code (fake acgame.library).
#   tests/amiga/test_host.sh A4_DIR     (gfopenallegro/allegro4)
# Copyright (c) 2026 AmiCradle Team. MIT, as Netacka.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
A4=${1:?gfopenallegro/allegro4 directory}
T=${TMPDIR:-/tmp}/netacka-host-$$
mkdir -p "$T"
trap 'rm -rf "$T"' EXIT HUP INT TERM
CC=${HOSTCC:-cc}
NET="$ROOT/libnet/lib"
mkdir -p "$T/net"
for f in "$NET"/core/*.c nonet wsockdos template ipx ipxsocks serial serdos serlinux local internet inetaddr; do
    case $f in */*) src=$f ;; *) src="$NET/drivers/$f.c" ;; esac
    "$CC" -w -O1 -I"$ROOT/libnet/include" -I"$NET/include" -c "$src" -o "$T/net/$(basename "$src" .c).o"
done
SRCS=$(ls "$A4"/src/a4_*.c | grep -v -e a4_os_amiga -e a4_amiga)
"$CC" -std=gnu99 -D_DEFAULT_SOURCE -w -O1 -I"$A4/include" -I"$A4/src" -I"$A4/tests" -I"$ROOT/libnet/include" \
  $SRCS "$A4"/tests/fake_acgame.c "$ROOT/tests/amiga/test_net.c" "$T"/net/*.o -o "$T/test_net"
"$T/test_net" "$ROOT"
echo "host tests: PASS"
