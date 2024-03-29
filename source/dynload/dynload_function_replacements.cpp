#include "dynload_function_replacements.h"
#include "globals.h"
#include "loader_defines.h"
#include "logger.h"
#include <coreinit/dynload.h>
#include <coreinit/title.h>
#include <cstdio>
#include <cstring>
#include <wums.h>

DECL_FUNCTION(void, OSDynLoad_Release, OSDynLoad_Module module) {
    if (((uint32_t) module & MODULE_MAGIC_MASK) == MODULE_MAGIC) {
        uint32_t moduleHandle = ((uint32_t) module) & MODULE_ID_MASK;
        if (moduleHandle >= gModuleData->number_modules) {
            DEBUG_FUNCTION_LINE_WARN("Invalid module handle was encoded in OSDynLoad_Module %d (%08X)", moduleHandle, module);
        } else {
            return;
        }
    }
    real_OSDynLoad_Release(module);
}

DECL_FUNCTION(OSDynLoad_Error, OSDynLoad_Acquire, char const *name, OSDynLoad_Module *outModule) {
    DEBUG_FUNCTION_LINE_VERBOSE("Looking for module %s", name);
    for (uint32_t i = 0; i < gModuleData->number_modules; i++) {
        if (strcmp(name, gModuleData->modules[i].module_export_name) == 0) {
            *outModule = (OSDynLoad_Module) (MODULE_MAGIC | i);
            return OS_DYNLOAD_OK;
        }
    }

    if (name != nullptr && name[0] == '~') {
        char cpy[64] = {};
        snprintf(cpy, sizeof(cpy), "%s", name);
        char *curPtr = &cpy[1];
        while (*curPtr != '\0') {
            if (*curPtr == '/') {
                *curPtr = '|';
            }
            curPtr++;
        }
        return real_OSDynLoad_Acquire(cpy, outModule);
    }

    OSDynLoad_Error result = real_OSDynLoad_Acquire(name, outModule);

    return result;
}

DECL_FUNCTION(OSDynLoad_Error, OSDynLoad_FindExport, OSDynLoad_Module module, BOOL isData, char const *name, void **outAddr) {
    OSDynLoad_Error result = real_OSDynLoad_FindExport(module, isData, name, outAddr);
    if (result == OS_DYNLOAD_OK) {
        return OS_DYNLOAD_OK;
    }

    if (((uint32_t) module & MODULE_MAGIC_MASK) == MODULE_MAGIC) {
        uint32_t moduleHandle = ((uint32_t) module) & MODULE_ID_MASK;
        if (moduleHandle >= gModuleData->number_modules) {
            DEBUG_FUNCTION_LINE_ERR("Invalid module handle was encoded in OSDynLoad_Module %d (%08X)", moduleHandle, module);
            return result;
        }

        auto *curModule = &gModuleData->modules[moduleHandle];
        DEBUG_FUNCTION_LINE_VERBOSE("Try to find export %s in module %s", name, curModule->module_export_name);
        for (uint32_t i = 0; i < curModule->number_export_entries; i++) {
            auto *curExport = &curModule->export_entries[i];
            if (strcmp(name, curExport->name) == 0) {
                if (isData && curExport->type == WUMS_FUNCTION_EXPORT) {
                    DEBUG_FUNCTION_LINE_ERR("Requested data Export but only found a function export");
                    return OS_DYNLOAD_INVALID_MODULE_NAME;
                }
                *outAddr = (void *) curExport->address;
                DEBUG_FUNCTION_LINE_VERBOSE("SUCCESS! Set outAddr to %08X. It's from module %s function %s",
                                            curExport->address,
                                            curModule->module_export_name,
                                            curExport->name);
                return OS_DYNLOAD_OK;
            }
        }
    }
    return result;
}

RPLFileInfo_v4_2 fileInfoBuffer;

DECL_FUNCTION(LOADED_RPL *, LiFindRPLByName, char *name) {
    for (uint32_t i = 0; i < gModuleData->number_modules; i++) {
        auto *curModule = &gModuleData->modules[i];
        if (strcmp(name, curModule->module_export_name) == 0) {
            fileInfoBuffer.tlsModuleIndex    = 0;
            gLoadedRPLData[i].fileInfoBuffer = &fileInfoBuffer; // will be copied to the LiImportTracking array
            gLoadedRPLData[i].loadStateFlags = 0x0;
            gLoadedRPLData[i].entrypoint     = 0x1; //needs to be != 0;
            gLoadedRPLData[i].funcExports    = (Export *) (FUNCTION_EXPORT_MAGIC + i);
            gLoadedRPLData[i].numFuncExports = 1;
            gLoadedRPLData[i].dataExports    = (Export *) (DATA_EXPORT_MAGIC + i);
            gLoadedRPLData[i].numDataExports = 1;
            return &gLoadedRPLData[i];
        }
    }
    return real_LiFindRPLByName(name);
}

DECL_FUNCTION(uint32_t, __OSDynLoad_InternalAcquire, char *name, RPL_DATA **out, uint32_t u1, uint32_t u2, uint32_t u3) {
    for (uint32_t i = 0; i < gModuleData->number_modules; i++) {
        auto *curModule = &gModuleData->modules[i];
        if (strcmp(name, curModule->module_export_name) == 0) {
            // OSDynLoad_IsModuleLoaded uses __OSDynLoad_InternalAcquire and expects out have a valid value.
            // It uses the "handle", so don't need to fill the whole struct.
            gRPLData[i].handle = MODULE_MAGIC | i;
            *out               = &gRPLData[i];
            return 0;
        }
    }
    return real___OSDynLoad_InternalAcquire(name, out, u1, u2, u3);
}

Export ourExportThing;

DECL_FUNCTION(Export *, LiBinSearchExport, Export *exports, int numExports, char *name) {
    auto isFunc = (((uint32_t) exports) & EXPORT_MASK) == FUNCTION_EXPORT_MAGIC;
    auto isData = (((uint32_t) exports) & EXPORT_MASK) == DATA_EXPORT_MAGIC;
    if (isFunc || isData) {
        uint32_t moduleHandle = ((uint32_t) exports) & EXPORT_MAGIC_MASK;
        if (moduleHandle > gModuleData->number_modules) {
            DEBUG_FUNCTION_LINE_LOADER_ERR("Invalid module handle was encoded in Export %d (%08X)", moduleHandle, exports);
            return nullptr;
        }

        auto *curModule = &gModuleData->modules[moduleHandle];
        DEBUG_FUNCTION_LINE_LOADER_VERBOSE("Try to find export %s in module %s", name, curModule->module_export_name);
        for (uint32_t i = 0; i < curModule->number_export_entries; i++) {
            auto *curExport = &curModule->export_entries[i];
            if (strcmp(name, curExport->name) == 0) {
                if ((isFunc && curExport->type == WUMS_FUNCTION_EXPORT) || (isData && curExport->type == WUMS_DATA_EXPORT)) {
                    ourExportThing.value = curExport->address;
                    ourExportThing.name  = 0;
                    return &ourExportThing;
                }
            }
        }
        return nullptr;
    }
    return real_LiBinSearchExport(exports, numExports, name);
}

DECL_FUNCTION(int32_t, LiFixupRelocOneRPL, LOADED_RPL *rpl, void *imports, uint32_t unknown) {
    auto rplAddress = (uint32_t) rpl;
    if (rplAddress >= (uint32_t) &gLoadedRPLData[0] && rplAddress < (uint32_t) &gLoadedRPLData[gModuleData->number_modules]) {
        // Skip if this is our fake RPL
        return 0;
    }
    return real_LiFixupRelocOneRPL(rpl, imports, unknown);
}

DECL_FUNCTION(int32_t, sCheckOne, LOADED_RPL *rpl) {
    auto rplAddress = (uint32_t) rpl;
    if (rplAddress >= (uint32_t) &gLoadedRPLData[0] && rplAddress < (uint32_t) &gLoadedRPLData[gModuleData->number_modules]) {
        // Skip if this is our fake RPL
        return 0;
    }
    return real_sCheckOne(rpl);
}

DECL_FUNCTION(void, sACPLoadOnDone, void) {
    if (OSGetTitleID() == 0x0005000010140900L) { // オセロ (Othello)
        DEBUG_FUNCTION_LINE_INFO("Skip sACPLoadOnDone for オセロ (Othello) as it might slow down exiting.");
        // For some unknown reason unloading the nn_acp.rpl after playing Othello
        // takes 30-100 seconds when many plugins are loaded... We take the very hacky and lazy route
        // and just stop calling it and pray this won't break anything.
        return;
    }

    return real_sACPLoadOnDone();
}

function_replacement_data_t dynload_function_replacements[] = {
        REPLACE_FUNCTION_VIA_ADDRESS(sACPLoadOnDone, 0x3201C400 + 0x29de0, 0x101C400 + 0x29de0),
        REPLACE_FUNCTION_VIA_ADDRESS(__OSDynLoad_InternalAcquire, 0x3201C400 + 0x0cc54, 0x101C400 + 0x0cc54),
        REPLACE_FUNCTION_VIA_ADDRESS(LiFindRPLByName, 0x32004BC4, 0x01004bc4),
        REPLACE_FUNCTION_VIA_ADDRESS(LiBinSearchExport, 0x320002f8, 0x010002f8),
        REPLACE_FUNCTION_VIA_ADDRESS(sCheckOne, 0x32007294, 0x01007294),
        REPLACE_FUNCTION_VIA_ADDRESS(LiFixupRelocOneRPL, 0x320059f0, 0x010059f0),
        REPLACE_FUNCTION(OSDynLoad_Acquire, LIBRARY_COREINIT, OSDynLoad_Acquire),
        REPLACE_FUNCTION(OSDynLoad_FindExport, LIBRARY_COREINIT, OSDynLoad_FindExport),
        REPLACE_FUNCTION(OSDynLoad_Release, LIBRARY_COREINIT, OSDynLoad_Release),
};

uint32_t dynload_function_replacements_size = sizeof(dynload_function_replacements) / sizeof(function_replacement_data_t);