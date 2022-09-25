#ifndef _HUGOS_STACK_H
#define _HUGOS_STACK_H

#include <stdint.h>
#include <stdio.h>
#include "allocator.h"

extern "C" struct Stack {
	public:
	uint64_t top;
	uint64_t bottom;

	Stack(uint64_t size, bool user_access = false);
	Stack(uint64_t top, uint64_t bottom): top(top), bottom(bottom) {}
	~Stack();
};

#endif