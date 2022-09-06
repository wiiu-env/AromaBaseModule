#pragma once

#include <stdint.h>

extern uint32_t gHeapMask;
extern uint32_t gNonHomebrewFSClientCount;

void initCommonPatches();
void commonPatchesStart();