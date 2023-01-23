#ifndef CREATORPKGS_H
#define CREATORPKGS_H

#include "state.h"
#include "read_write.h"
#include "identification.h"
#include "header.h"

struct Header* createPkgs(uint8_t* data, uint64_t size, bool *error,
                          uint64_t *expectedDataSize, uint64_t *cutDataSize);

struct Header* createHeader(uint8_t* data, uint64_t size, bool *error,
                            uint64_t *expectedDataSize, uint64_t *cutDataSize);

struct Identification* createIdentification(uint8_t* const data, uint64_t size, bool *error,
                                    uint64_t *expectedDataSize, uint64_t *cutDataSize);

struct State* createState(uint8_t* const data, uint64_t size, bool *error,
                          uint64_t *expectedDataSize, uint64_t *cutDataSize);

struct Read_Write* createReadWrite(uint8_t* const data, uint64_t size, bool *error,
                                   uint64_t *expectedDataSize, uint64_t *cutDataSize);

bool bodyMustBe(uint8_t assigment);

bool isLittleEndian();
void dataReverse(void* data, uint64_t size);

#endif // CREATORPKGS_H
