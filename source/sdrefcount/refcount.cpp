#include "globals.h"
#include "logger.h"
#include "sd_function_replacements.h"
#include <coreinit/filesystem_fsa.h>

void initSDRefCount() {
    FSAInit();
    auto client      = FSAAddClient(nullptr);
    gSDMountRefCount = 0;
    if (client > 0) {
        auto res = FSAMount(client, "/dev/sdcard01", "/vol/external01", (FSAMountFlags) 0, nullptr, 0);
        if (res == FS_ERROR_ALREADY_EXISTS) {
            DEBUG_FUNCTION_LINE("SDCard is already mounted, set ref counter to 16");
            gSDMountRefCount = 16;
        } else {
            DEBUG_FUNCTION_LINE("Set ref counter to 0");
            FSAUnmount(client, "/vol/external01", FSA_UNMOUNT_FLAG_BIND_MOUNT);
        }
        FSADelClient(res);
    }

    DEBUG_FUNCTION_LINE("Patch SDRefCount functions");
    for (uint32_t i = 0; i < sdrefcount_function_replacements_size; i++) {
        if (!FunctionPatcherPatchFunction(&sdrefcount_function_replacements[i], nullptr)) {
            OSFatal("AromaBaseModule: Failed to patch function for sd ref counting");
        }
    }
    DEBUG_FUNCTION_LINE("Patch SDRefCount functions finished");
}