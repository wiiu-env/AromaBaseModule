#include "globals.h"
#include "logger.h"
#include "patches_replacements.h"
#include <coreinit/cache.h>
#include <cstdlib>
#include <kernel/kernel.h>

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
        gHeapMask = (uint32_t) dummy & 0xF0000000;
        free(dummy);
    }
    gNonHomebrewFSClientCount = 0;

    DEBUG_FUNCTION_LINE("Do common patches");
    for (uint32_t i = 0; i < patches_function_replacements_size; i++) {
        bool wasPatched = false;
        if (FunctionPatcher_AddFunctionPatch(&patches_function_replacements[i], nullptr, &wasPatched) != FUNCTION_PATCHER_RESULT_SUCCESS || !wasPatched) {
            OSFatal("AromaBaseModule: Failed apply common patches");
        }
    }
    DEBUG_FUNCTION_LINE("Common patches finished");

    // Patch loader.elf to spit out less warnings when loading .rpx built with wut
    KernelNOPAtPhysicalAddress(0x0100b770 - 0x01000000 + 0x32000000);
    KernelNOPAtPhysicalAddress(0x0100b800 - 0x01000000 + 0x32000000);
    KernelNOPAtPhysicalAddress(0x0100b7b8 - 0x01000000 + 0x32000000);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b770), 0x04);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b800), 0x04);
    ICInvalidateRange(reinterpret_cast<void *>(0x0100b7b8), 0x04);
}

void commonPatchesStart() {
    gNonHomebrewFSClientCount = 0;
}