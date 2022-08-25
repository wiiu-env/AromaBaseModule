#include "dynload.h"
#include "../globals.h"
#include "dynload_function_replacements.h"
#include "logger.h"
#include <malloc.h>

void initDynload() {
    gRPLData = (LOADED_RPL *) malloc(sizeof(LOADED_RPL) * gModuleData->number_modules);
    if (!gRPLData) {
        DEBUG_FUNCTION_LINE_ERR("Failed to allocate gRPLData");
        OSFatal("AromaBaseModule: Failed to allocate gRPLData");
    }

    DEBUG_FUNCTION_LINE("Patch functions for dynload patches");
    for (uint32_t i = 0; i < dynload_function_replacements_size; i++) {
        if (!FunctionPatcherPatchFunction(&dynload_function_replacements[i], nullptr)) {
            OSFatal("AromaBaseModule: Failed to patch function for dynload patches");
        }
    }
    DEBUG_FUNCTION_LINE("Patch functions finished for dynload patches");
}