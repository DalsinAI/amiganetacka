/* Netacka for GFOS, host test: libnet built the way amiga/Makefile.amigaos
 * builds it (no TARGET_*, so BSD sockets; every driver file) carries the
 * game's packets: a server channel bound as Netacka binds it, a client
 * channel aimed at it, both ways, over the loopback; the local driver
 * too. Then netacka.ini read through gfopenallegro's config calls, as
 * set_the_damn_config() and main() read it.
 *   test_net GAME_DIR
 * Copyright (c) 2026 AmiCradle Team. MIT, as Netacka. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libnet.h>
#include <allegro.h>

static int fails, checks;
#define CHECK(c) do { checks++; if (!(c)) { fails++; printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); } } while (0)

static int wait_for(NET_CHANNEL *c)
{
    int i;
    for (i = 0; i < 200; i++) {
        if (net_query(c)) return 1;
        usleep(5000);
    }
    return 0;
}

static void round_trip(int driver, const char *bind, const char *target)
{
    char from[NET_MAX_ADDRESS_LENGTH], buf[64];
    unsigned char hello[10] = { 3, 2, 's', 'e', 'c', 'r', 'e', 't', 0, 0 };
    NET_CHANNEL *srv, *cli;
    int n;

    CHECK(net_initdriver(driver) != 0);
    srv = net_openchannel(driver, bind);
    cli = net_openchannel(driver, 0);
    CHECK(srv != NULL && cli != NULL);
    if (!srv || !cli) return;
    CHECK(net_assigntarget(cli, target) == 0);
    /* clIWANNAPLAY: id, players, password, NUL (send_client_players) */
    CHECK(net_send(cli, hello, 9) == 0);
    CHECK(wait_for(srv));
    n = net_receive(srv, buf, sizeof(buf), from);
    CHECK(n == 9 && memcmp(buf, hello, 9) == 0);
    /* the server answers whoever spoke, as netacka.c does */
    CHECK(net_assigntarget(srv, from) == 0);
    CHECK(net_send(srv, "\x07", 1) == 0);
    CHECK(wait_for(cli));
    n = net_receive(cli, buf, sizeof(buf), 0);
    CHECK(n == 1 && buf[0] == 7);
    net_closechannel(cli);
    net_closechannel(srv);
}

int main(int argc, char **argv)
{
    char path[1024];
    NET_DRIVERLIST d;
    NET_DRIVERNAME *names;

    if (argc < 2) return 2;
    CHECK(net_init() == 0);
    /* start_net(): the first internet-class driver found */
    d = net_detectdrivers(net_classes[NET_CLASS_INET].drivers);
    names = net_getdrivernames(d);
    CHECK(names && names[0].num == NET_DRIVER_SOCKETS);
    free(names);
    round_trip(NET_DRIVER_SOCKETS, "0.0.0.0:16789", "127.0.0.1:16789");
    round_trip(NET_DRIVER_LOCAL, "16790", "16790");
    net_shutdown();

    CHECK(allegro_init() == 0);
    snprintf(path, sizeof(path), "%s/netacka.ini", argv[1]);
    set_config_file(path);
    CHECK(get_config_int(0, "windowed", -1) == 1);
    CHECK(strcmp(get_config_string("client", "server", ""), "localhost:6789") == 0);
    CHECK(get_config_int("server", "port", -1) == 6789);
    CHECK(get_config_int("server", "screen_w", -1) == 800);
    CHECK(get_config_int("server", "fps", -1) == 35);

    printf("%d checks, %d failed\n", checks, fails);
    return fails ? 1 : 0;
}
