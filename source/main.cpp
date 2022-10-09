#include "applicationendshook/applicationendhook.h"
#include "dynload/dynload.h"
#include "globals.h"
#include "logger.h"
#include "patches/patches.h"
#include "sdrefcount/refcount.h"
#include "symbolnamepatcher/symbolname.h"
#include "version.h"
#include <wums.h>

WUMS_MODULE_EXPORT_NAME("homebrew_basemodule");
WUMS_MODULE_SKIP_INIT_FINI();

#define VERSION "v0.2.0"

WUMS_INITIALIZE(args) {
    initLogging();
    gModuleData = args.module_information;
    if (gModuleData == nullptr) {
        OSFatal("AromaBaseModule: Failed to get gModuleData pointer.");
    }
    if (gModuleData->version != MODULE_INFORMATION_VERSION) {
        OSFatal("AromaBaseModule: The module information struct version does not match.");
    }

    initApplicationEndsHook();
    initSDRefCount();
    initSymbolNamePatcher();
    initDynload();
    initCommonPatches();

    deinitLogging();
}

WUMS_APPLICATION_STARTS() {
    OSReport("Running AromaBaseModule " VERSION VERSION_EXTRA "\n");

    commonPatchesStart();
}
