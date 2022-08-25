#include "applicationendhook.h"
#include "applicationends_function_replacements.h"
#include "logger.h"


void initApplicationEndsHook() {
    DEBUG_FUNCTION_LINE("Patch ApplicationEndsHook functions");
    for (uint32_t i = 0; i < applicationendshook_function_replacements_size; i++) {
        if (!FunctionPatcherPatchFunction(&applicationendshook_function_replacements[i], nullptr)) {
            OSFatal("AromaBaseModule: Failed to patch ApplicationEndsHook function");
        }
    }
    DEBUG_FUNCTION_LINE("Patch ApplicationEndsHook functions finished");
}
