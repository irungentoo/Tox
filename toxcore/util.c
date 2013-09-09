/*
 * util.c -- Utilities.
 *
 * This file is donated to the Tox Project.
 * Copyright 2013  plutooo
 */

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#include "DHT.h"
#include "packets.h"

long int now()
{
    return time(NULL);
}

uint64_t random_64b()
{
    uint64_t r;

    /* This is probably not random enough? */
    r = random_int();
    r <<= 32;
    r |= random_int();

    return r;
}

bool ipp_eq(IP_Port a, IP_Port b)
{
    return (a.ip.i == b.ip.i) && (a.port == b.port);
}

bool id_eq(clientid_t *dest, clientid_t *src)
{
    return memcmp(dest, src, sizeof(clientid_t)) == 0;
}

void id_cpy(clientid_t *dest, clientid_t *src)
{
    memcpy(dest, src, sizeof(clientid_t));
}
