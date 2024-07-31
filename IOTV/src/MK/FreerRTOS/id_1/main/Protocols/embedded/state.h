#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct State
{
    uint8_t nameSize;
    uint8_t state;
    uint8_t flags;

    uint64_t dataSize;

    char *name;
    uint8_t *data;
};
typedef struct State state_pkg_t;

uint64_t stateCheckSum(const state_pkg_t *body);
uint64_t stateSize(const state_pkg_t *body);
uint64_t stateToData(const state_pkg_t *body, char *outData, uint64_t outDataSize);
void clearState(state_pkg_t *state);

state_pkg_t *stateCopy(const state_pkg_t *state_pkg);

#ifdef __cplusplus
}
#endif

#endif // STATE_H
