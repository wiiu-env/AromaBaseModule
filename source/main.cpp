#include "applicationendshook/applicationendhook.h"
#include "dynload/dynload.h"
#include "globals.h"
#include "logger.h"
#include "sdrefcount/refcount.h"
#include "symbolnamepatcher/symbolname.h"
#include <wums.h>

WUMS_MODULE_EXPORT_NAME("homebrew_basemodule");
WUMS_MODULE_SKIP_INIT_FINI();

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

    deinitLogging();
}
