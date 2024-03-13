#ifndef CREATORPKGS_H
#define CREATORPKGS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "state.h"
#include "read_write.h"
#include "identification.h"
#include "tech.h"
#include "header.h"

// Формирование пакетов из сырых данных

struct Header* createPkgs(uint8_t *data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize);
struct Header* createHeader(uint8_t *data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize);
struct Identification* createIdentification(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize);
struct State* createState(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize);
struct Read_Write* createReadWrite(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize);
struct Tech* createTech(uint8_t *data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize);

// Проверка для header c заданным assigment, должно ли быть тело пакета
bool isBodyMustBe(uint8_t type, uint8_t assigment);

bool isLittleEndian();
void dataReverse(void *data, uint64_t size);

#endif // CREATORPKGS_H
