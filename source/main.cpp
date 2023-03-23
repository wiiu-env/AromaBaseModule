#include "applicationendshook/applicationendhook.h"
#include "dynload/dynload.h"
#include "globals.h"
#include "logger.h"
#include "patches/patches.h"
#include "sdrefcount/refcount.h"
#include "symbolnamepatcher/symbolname.h"
#include "version.h"
#include <function_patcher/function_patching.h>
#include <wums.h>

WUMS_MODULE_EXPORT_NAME("homebrew_basemodule");
WUMS_MODULE_SKIP_INIT_FINI();
WUMS_DEPENDS_ON(homebrew_functionpatcher);

#define VERSION "v0.2.2"

WUMS_INITIALIZE(args) {
    initLogging();
    gModuleData = args.module_information;
    if (gModuleData == nullptr) {
        OSFatal("AromaBaseModule: Failed to get gModuleData pointer.");
    }
    if (gModuleData->version != MODULE_INFORMATION_VERSION) {
        OSFatal("AromaBaseModule: The module information struct version does not match.");
    }

    // First init Dynload to have proper OSDynLoad support!
    initDynload();

    // Now init the library so we can use it for the other patches.
    if (FunctionPatcher_InitLibrary() != FUNCTION_PATCHER_RESULT_SUCCESS) {
        OSFatal("homebrew_basemodule: FunctionPatcher_InitLibrary failed");
    }

    initApplicationEndsHook();
    initSDRefCount();
    initSymbolNamePatcher();

    initCommonPatches();

    deinitLogging();
}

WUMS_APPLICATION_STARTS() {
    OSReport("Running AromaBaseModule " VERSION VERSION_EXTRA "\n");

    commonPatchesStart();
}
