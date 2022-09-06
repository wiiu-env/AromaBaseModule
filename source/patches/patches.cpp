#include "globals.h"
#include "logger.h"
#include "patches_replacements.h"
#include <cstdlib>

// init is not called for this module. We need to make sure to init these values in initCommonPatches()
uint32_t gHeapMask;
uint32_t gNonHomebrewFSClientCount;

void initCommonPatches() {
    // Determine which region the memory mapping is using. We need this to count the
    // number of FSClients which are stored in that region.
    auto dummy = malloc(0x4);
    if (!dummy) {
        gHeapMask = 0x80000000;
    } else {
        DEBUG_FUNCTION_LINE_ERR("%08X", dummy);
        gHeapMask = (uint32_t) dummy & 0xF0000000;
        free(dummy);
    }
    gNonHomebrewFSClientCount = 0;

    DEBUG_FUNCTION_LINE("Do common patches");
    for (uint32_t i = 0; i < patches_function_replacements_size; i++) {
        if (!FunctionPatcherPatchFunction(&patches_function_replacements[i], nullptr)) {
            OSFatal("AromaBaseModule: Failed apply common patches");
        }
    }
    DEBUG_FUNCTION_LINE("Common patches finished");
}

void commonPatchesStart() {
    gNonHomebrewFSClientCount = 0;
}