#ifndef nol_vm_h
#define nol_vm_h

#include "common.h"

#define STACK_MAX 256

void init_vm();
void free_vm();
bool run_code(uint8_t* code);

#endif