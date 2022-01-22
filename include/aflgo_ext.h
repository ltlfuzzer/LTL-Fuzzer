#include <stdint.h>
#include <shmdata.h>

#ifndef AFLGO_EXT_H
#define AFLGO_EXT_H
/*
 * This funciton replaces the prefix of generated inputs. The prefix is obtained 
 * from shared memory.
 * 
 * parameters: 
 * void* mem: the memory block containing the generated input by aflgo 
 *
 * uint32_t len: the size of the generated input
 *
 */

void replace_with_prefix(void* mem, uint32_t len, PREFIX_SMEM* shm);
void add_prefix_common(void** mem, uint32_t* len, PREFIX_SMEM* shm);

#endif
