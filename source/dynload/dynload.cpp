#include "dynload.h"
#include "../globals.h"
#include "dynload_function_replacements.h"
#include "logger.h"
#include <malloc.h>

extern "C" FunctionPatcherStatus FPAddFunctionPatch(function_replacement_data_t *function_data, PatchedFunctionHandle *outHandle, bool *outHasBeenPatched);

void initDynload() {
    gLoadedRPLData = (LOADED_RPL *) malloc(sizeof(LOADED_RPL) * gModuleData->number_modules);
    if (!gLoadedRPLData) {
        DEBUG_FUNCTION_LINE_ERR("Failed to allocate gLoadedRPLData");
        OSFatal("AromaBaseModule: Failed to allocate gLoadedRPLData");
    }
    gRPLData = (RPL_DATA *) malloc(sizeof(RPL_DATA) * gModuleData->number_modules);
    if (!gRPLData) {
        DEBUG_FUNCTION_LINE_ERR("Failed to allocate gRPLData");
        OSFatal("AromaBaseModule: Failed to allocate gRPLData");
    }

    DEBUG_FUNCTION_LINE("Patch functions for dynload patches");
    for (uint32_t i = 0; i < dynload_function_replacements_size; i++) {
        bool wasPatched = false;
        // We need to use FPAddFunctionPatch because we can't use libfunctionpatcher yet. This patch enables it though.
        if (FPAddFunctionPatch(&dynload_function_replacements[i], nullptr, &wasPatched) != FUNCTION_PATCHER_RESULT_SUCCESS || !wasPatched) {
            OSFatal("AromaBaseModule: Failed to patch function for dynload patches");
        }
    }
    DEBUG_FUNCTION_LINE("Patch functions finished for dynload patches");
}