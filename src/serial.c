/*
serial.c
Serial Data Transfer
Contains functions that handle reading/writing from/to the LCD registers

Copyright (C) 2020 akrocynova

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "logger.h"
#include "gameboy.h"
#include "cpu/interrupts.h"
#include <SDL_net.h>

#define SERIAL_CLOCKS(freq) (CPU_CLOCK_SPEED / (freq))
#define SERIAL_TCP_PORT (1989)

static IPaddress        server_ip     = {.host = INADDR_ANY, .port = 0};
static TCPsocket        server_socket = NULL;
static IPaddress        client_ip     = {.host = INADDR_NONE, .port = 0};
static TCPsocket        client_socket = NULL;
static SDLNet_SocketSet client_set    = NULL;

static byte_t           pkt_id        = 0;

bool serial_server_opened(void)
{
    return server_socket ? true : false;
}

bool serial_connected(void)
{
    return client_socket ? true : false;
}

byte_t serial_pkt_id(void)
{
    return pkt_id;
}

// Returns a statically allocated buffer
char *serial_remote_address(void)
{
    static char addr[64];

    snprintf(addr, sizeof(addr), "%s:%u", SDLNet_ResolveIP(&client_ip), client_ip.port);
    return addr;
}

// Disconnect and free link cable resources
void serial_quit(void)
{
    if (server_socket)
        SDLNet_TCP_Close(server_socket);
    if (client_socket)
        SDLNet_TCP_Close(client_socket);
    if (client_set)
        SDLNet_FreeSocketSet(client_set);
}

// Initialize link cable emulation
// Returns 0 on success, -1 on error
int serial_init()
{
    if (!(client_set = SDLNet_AllocSocketSet(1))) {
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return -1;
    }
    return 0;
}

// Close link cable server
void serial_server_close(void)
{
    if (server_socket) {
        SDLNet_TCP_Close(server_socket);
        server_socket = NULL;
    }
}

// Open link cable server
void serial_server_open(void)
{
    SDLNet_ResolveHost(&server_ip, NULL, SERIAL_TCP_PORT);
    if (!server_socket && !(server_socket = SDLNet_TCP_Open(&server_ip)))
        logger(LOG_ERROR, "SDLNet_TCP_Open: %s", SDLNet_GetError());
}

void serial_server_accept(void)
{
    // Accept incoming connections
    if (!client_socket && server_socket) {
        if ((client_socket = SDLNet_TCP_Accept(server_socket))) {
            client_ip = *SDLNet_TCP_GetPeerAddress(client_socket);
            SDLNet_TCP_AddSocket(client_set, client_socket);
        }
    }
}

// Disconnect the link cable
void serial_disconnect(gb_system_t *gb)
{
    pkt_id = 0;
    gb->serial.shift_clock = 0;
    gb->serial.shifts = 0;
    SDLNet_TCP_DelSocket(client_set, client_socket);
    SDLNet_TCP_Close(client_socket);
    client_socket = NULL;
}

// Connect to remote link cable server
bool serial_connect(const char *hostname)
{
    if (!server_socket && !client_socket) {
        if (SDLNet_ResolveHost(&client_ip, hostname, SERIAL_TCP_PORT) < 0) {
            logger(LOG_ERROR, "SDLNet_ResolveHost: %s", SDLNet_GetError());
            return false;
        }

        if (!(client_socket = SDLNet_TCP_Open(&client_ip))) {
            logger(LOG_ERROR, "SDLNet_TCP_Open: %s", SDLNet_GetError());
            return false;
        }
        SDLNet_TCP_AddSocket(client_set, client_socket);
    }
    return true;
}

// Emulate internal transfer between two emulators
void serial_transfer_internal(gb_system_t *gb)
{
    byte_t link_pkt[2] = {pkt_id, gb->serial.sb};
    const int pkt_size = sizeof(link_pkt);

    // Send local data and receive remote data
    if (   SDLNet_TCP_Send(client_socket, link_pkt, pkt_size) == pkt_size
        && SDLNet_TCP_Recv(client_socket, link_pkt, pkt_size) == pkt_size)
    {
        // Check if the received packet is valid
        if (link_pkt[0] == pkt_id) {
            pkt_id += 1;
            logger(LOG_DEBUG, "Serial OUT: %02X", gb->serial.sb);
            logger(LOG_DEBUG, "Serial IN: %02X", link_pkt[1]);
            gb->serial.sb = link_pkt[1];
            gb->serial.sc.transfer_start = 0;
            cpu_int_flag_set(INT_SERIAL_BIT, gb);
        } else {
            logger(LOG_ERROR, "serial_transfer_internal: got id %u but expected %u", link_pkt[0], pkt_id);
            serial_disconnect(gb);
        }
    } else {
        logger(LOG_ERROR, "serial_transfer_internal: %s", SDLNet_GetError());
        serial_disconnect(gb);
    }
}

// Emulate external transfer between two emulators
void serial_transfer_external(gb_system_t *gb)
{
    byte_t link_pkt[2];
    const int pkt_size = sizeof(link_pkt);
    int status;

    status = SDLNet_CheckSockets(client_set, 0);
    if (   status > 0
        && SDLNet_SocketReady(client_socket))
    {
        // Receive pending packet
        if (SDLNet_TCP_Recv(client_socket, link_pkt, pkt_size) == pkt_size) {
            // Check if the received packet is valid
            if (link_pkt[0] == pkt_id) {
                // Store the remote data
                gb->serial.sb_in = link_pkt[1];

                // Prepare the next packet with local data
                link_pkt[1] = gb->serial.sb;

                // Send the local data to the remote emulator
                if (SDLNet_TCP_Send(client_socket, link_pkt, pkt_size) == pkt_size) {
                    pkt_id += 1;
                    logger(LOG_DEBUG, "Serial OUT: %02X", gb->serial.sb);
                    logger(LOG_DEBUG, "Serial IN: %02X", gb->serial.sb_in);
                    gb->serial.sb = gb->serial.sb_in;
                    gb->serial.sc.transfer_start = 0;
                    cpu_int_flag_set(INT_SERIAL_BIT, gb);
                } else {
                    logger(LOG_ERROR, "serial_transfer_external (send): %s", SDLNet_GetError());
                    serial_disconnect(gb);
                }
            } else {
                logger(LOG_ERROR, "serial_transfer_external: got id %u but expected %u", link_pkt[0], pkt_id);
                serial_disconnect(gb);
            }
        } else {
            logger(LOG_ERROR, "serial_transfer_external (recv): %s", SDLNet_GetError());
            serial_disconnect(gb);
        }
    } else if (status < 0) {
        logger(LOG_ERROR, "serial_transfer_external (check): %s", SDLNet_GetError());
        serial_disconnect(gb);
    }
}

byte_t serial_reg_readb(uint16_t addr, gb_system_t *gb)
{
    switch (addr) {
        case SERIAL_SB: return gb->serial.sb;
        case SERIAL_SC: return *((byte_t *) &gb->serial.sc);
        default:
            logger(LOG_ERROR, "serial_reg_readb failed: unhandled address $%04X", addr);
            return MMU_UNMAPPED_ADDR_VALUE;
    }
}

bool serial_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr) {
        case SERIAL_SB:
            gb->serial.sb = value;
            return true;

        case SERIAL_SC:
            *((byte_t *) &gb->serial.sc) = (value | 0x7C);
            if (client_socket) {
                // Link cable is plugged
                if (gb->serial.sc.internal_clock && gb->serial.sc.transfer_start)
                    serial_transfer_internal(gb);
            } else {
                // Link cable is unplugged
                if (!gb->serial.sc.transfer_start) {
                    gb->serial.shift_clock = 0;
                    gb->serial.shifts = 0;
                }
                gb->serial.clock_speed = gb->serial.sc.internal_clock ? SERIAL_CLOCKS(SERIAL_FREQ) : 0;
            }
            return true;

        default:
            logger(LOG_ERROR, "serial_reg_writeb failed: unhandled address $%04X", addr);
            return false;
    }
}

// TODO: Add an option to dump the serial transfers
// Equivalent of cpu_cycle() for the Serial Port
void serial_cycle(gb_system_t *gb)
{
    if (!gb->serial.sc.transfer_start)
        return;

    if (client_socket) {
        // Link cable is plugged
        if (   !gb->serial.sc.internal_clock
            &&  (gb->serial.shift_clock += 1) >= (SERIAL_CLOCKS(SERIAL_FREQ) * 8))
        {
            gb->serial.shift_clock = 0;
            serial_transfer_external(gb);
        }
    } else {
        // Link cable is not plugged
        if (    gb->serial.clock_speed > 0
            && (gb->serial.shift_clock += 1) >= gb->serial.clock_speed)
        {
            gb->serial.shift_clock = 0;

            // First shift initializes the in/out bytes
            // When the serial port is not plugged, received bits are 1
            if (gb->serial.shifts == 0) {
                logger(LOG_DEBUG, "Serial OUT: %02X", gb->serial.sb);
                gb->serial.sb_in = 0xFF;
            }

            gb->serial.sb <<= 1; // Local bit out
            gb->serial.sb |= (gb->serial.sb_in >> 7); // Remote bit in
            gb->serial.sb_in <<= 1; // Remote bit out

            if ((gb->serial.shifts += 1) >= 8) {
                gb->serial.shifts = 0;
                gb->serial.sc.transfer_start = 0;
                logger(LOG_DEBUG, "Serial IN: %02X", gb->serial.sb);
                cpu_int_flag_set(INT_SERIAL_BIT, gb);
            }
        }
    }
}