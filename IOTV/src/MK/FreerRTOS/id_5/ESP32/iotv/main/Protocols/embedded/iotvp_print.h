#ifndef IOTVP_PRINT_H
#define IOTVP_PRINT_H

#include <assert.h>
#include <stdio.h>

#include "header.h"
#include "identification.h"
#include "state.h"
#include "read_write.h"

void printHeader(const struct Header *head);
void printIdentification(const struct Identification *ident);
void printState(const struct State *state);
void printReadWrite(const struct Read_Write*readWrite);

#endif // IOTVP_PRINT_H
