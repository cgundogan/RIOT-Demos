/*
 * Copyright (C) 2015 Cenk Gündoğan <cenk.guendogan@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @author      Cenk Gündoğan <cenk.guendogan@fu-berlin.de>
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "xtimer.h"
#include "isl29020.h"
#include "board.h"
#include "net/gnrc/rpl.h"

#define MODE            ISL29020_MODE_AMBIENT
#define RANGE           ISL29020_RANGE_16K
#define SLEEP           (1000 * SEC_IN_MS)
#define MY_ADDR         "dead:beef::3"
#define IFACE_ID        (6)

static int udp_send(char *addr_str, uint16_t port, uint16_t value)
{
    struct sockaddr_in6 dst = { .sin6_family = AF_INET6, .sin6_port = htons(port) };
    int s;

    /* parse destination address */
    inet_pton(AF_INET6, addr_str, &dst.sin6_addr);

    /* create socket */
    s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    value = htons(value);
    sendto(s, &value, sizeof(value), 0, (struct sockaddr *)&dst, sizeof(dst));
    close(s);
    return 0;
}


int main(void)
{
    isl29020_t dev;
    uint16_t value;

    gnrc_rpl_init(IFACE_ID);
    isl29020_init(&dev, ISL29020_I2C, ISL29020_ADDR, ISL29020_RANGE_16K, ISL29020_MODE_AMBIENT);

    while (1) {
        value = (uint16_t) isl29020_read(&dev);
        udp_send("dead:beef::1", 56830, value);
        xtimer_usleep(SLEEP);
    }

    return 0;
}
