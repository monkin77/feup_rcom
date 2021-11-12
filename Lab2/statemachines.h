#ifndef STATE_MACHINES_H
#define STATE_MACHINES_H

#include "common.h"

/**
 * Receives the state, the file descriptor, address byte, control byte and a buffer.
 * Reads a byte from the Serial Port and changes the state.
 * mem[0] = received address byte
 * mem[1] = received ctrl byte
 * mem[2] = calculated bcc
 * Returns 0 on success, and 1 otherwise
 */
int receive_supervision_machine(State* state, int fd, u_int8_t addr, u_int8_t ctrl, u_int8_t mem[]);

#endif
