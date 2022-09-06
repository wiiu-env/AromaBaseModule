#include "sd_function_replacements.h"
#include "globals.h"
#include "logger.h"
#include <coreinit/filesystem_fsa.h>
#include <string_view>

DECL_FUNCTION(void, __PPCExit, uint32_t u1) {
    if (gSDMountRefCount > 0) {
        FSAInit();
        auto client = FSAAddClient(nullptr);
        if (client) {
            FSAUnmount(client, "/vol/external01/", FSA_UNMOUNT_FLAG_BIND_MOUNT);
        }
        gSDMountRefCount = 0;
    }
    real___PPCExit(u1);
}

DECL_FUNCTION(FSStatus, FSMount, FSClient *client, FSCmdBlock *cmd, FSMountSource *source, char *target, uint32_t bytes, FSErrorFlag errorMask) {
    if (std::string_view(target) == "/vol/external01") {
        if (gSDMountRefCount > 0) {
            gSDMountRefCount++;
            return FS_STATUS_OK;
        }
        auto res = real_FSMount(client, cmd, source, target, bytes, errorMask);
        if (res == FS_STATUS_OK) {
            gSDMountRefCount++;
        }
        return res;
    }


    return real_FSMount(client, cmd, source, target, bytes, errorMask);
}

DECL_FUNCTION(FSStatus, FSUnmount, FSClient *client, FSCmdBlock *cmd, const char *target, FSErrorFlag errorMask) {
    if (std::string_view(target) == "/vol/external01") {
        gSDMountRefCount--;
        if (gSDMountRefCount <= 0) {
            gSDMountRefCount = 0;
            return real_FSUnmount(client, cmd, target, errorMask);
        }
        return FS_STATUS_OK;
    }
    return real_FSUnmount(client, cmd, target, errorMask);
}

DECL_FUNCTION(FSError, FSAMount, FSAClientHandle client, const char *source, const char *target, FSAMountFlags flags, void *arg_buf, uint32_t arg_len) {
    if (std::string_view(target) == "/vol/external01") {
        if (gSDMountRefCount > 0) {
            gSDMountRefCount++;
            return FS_ERROR_OK;
        }
        auto res = real_FSAMount(client, source, target, flags, arg_buf, arg_len);
        if (res == FS_ERROR_OK || res == FS_ERROR_ALREADY_EXISTS) {
            gSDMountRefCount++;
        }
        return res;
    }

    return real_FSAMount(client, source, target, flags, arg_buf, arg_len);
}

DECL_FUNCTION(FSError, FSAUnmount, FSAClientHandle client, const char *mountedTarget, FSAUnmountFlags flags) {
    if (std::string_view(mountedTarget) == "/vol/external01") {
        gSDMountRefCount--;
        if (gSDMountRefCount <= 0) {
            auto res         = real_FSAUnmount(client, mountedTarget, flags);
            gSDMountRefCount = 0;
            return res;
        }
        return FS_ERROR_OK;
    }

    return real_FSAUnmount(client, mountedTarget, flags);
}

function_replacement_data_t sdrefcount_function_replacements[] = {
        REPLACE_FUNCTION(__PPCExit, LIBRARY_COREINIT, __PPCExit),
        REPLACE_FUNCTION(FSMount, LIBRARY_COREINIT, FSMount),
        REPLACE_FUNCTION(FSUnmount, LIBRARY_COREINIT, FSUnmount),
        REPLACE_FUNCTION(FSAMount, LIBRARY_COREINIT, FSAMount),
        REPLACE_FUNCTION(FSAUnmount, LIBRARY_COREINIT, FSAUnmount)};

uint32_t sdrefcount_function_replacements_size = sizeof(sdrefcount_function_replacements) / sizeof(function_replacement_data_t);