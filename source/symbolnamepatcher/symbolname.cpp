#include "globals.h"
#include "logger.h"
#include "sym_function_replacements.h"

void initSymbolNamePatcher() {
    DEBUG_FUNCTION_LINE("Patch SymbolNamePatcher functions");
    for (uint32_t i = 0; i < symbolname_function_replacements_size; i++) {
        bool wasPatched = false;
        if (FunctionPatcher_AddFunctionPatch(&symbolname_function_replacements[i], nullptr, &wasPatched) != FUNCTION_PATCHER_RESULT_SUCCESS || !wasPatched) {
            OSFatal("AromaBaseModule: Failed to patch SymbolNamePatcher function");
        }
    }
    DEBUG_FUNCTION_LINE("Patch SymbolNamePatcher functions finished");
}