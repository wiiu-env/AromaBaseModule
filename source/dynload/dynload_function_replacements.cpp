#include "dynload_function_replacements.h"
#include "globals.h"
#include "loader_defines.h"
#include "logger.h"
#include <coreinit/dynload.h>
#include <cstring>
#include <wums.h>

DECL_FUNCTION(OSDynLoad_Error, OSDynLoad_Acquire, char const *name, OSDynLoad_Module *outModule) {
    DEBUG_FUNCTION_LINE_VERBOSE("Looking for module %s", name);
    for (uint32_t i = 0; i < gModuleData->number_modules; i++) {
        if (strcmp(name, gModuleData->modules[i].module_export_name) == 0) {
            *outModule = (OSDynLoad_Module) (MODULE_MAGIC | i);
            return OS_DYNLOAD_OK;
        }
    }

    OSDynLoad_Error result = real_OSDynLoad_Acquire(name, outModule);
    if (result == OS_DYNLOAD_OK) {
        return OS_DYNLOAD_OK;
    }

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
            fileInfoBuffer.tlsModuleIndex = 0;
            gRPLData[i].fileInfoBuffer    = &fileInfoBuffer; // will be copied to the LiImportTracking array
            gRPLData[i].loadStateFlags    = 0x0;
            gRPLData[i].entrypoint        = 0x1; //needs to be != 0;
            gRPLData[i].funcExports       = (Export *) (FUNCTION_EXPORT_MAGIC + i);
            gRPLData[i].numFuncExports    = 1;
            gRPLData[i].dataExports       = (Export *) (DATA_EXPORT_MAGIC + i);
            gRPLData[i].numDataExports    = 1;
            return &gRPLData[i];
        }
    }
    return real_LiFindRPLByName(name);
}

DECL_FUNCTION(uint32_t, __OSDynLoad_InternalAcquire, char *name, void *out, uint32_t u1, uint32_t u2, uint32_t u3) {
    for (uint32_t i = 0; i < gModuleData->number_modules; i++) {
        auto *curModule = &gModuleData->modules[i];
        if (strcmp(name, curModule->module_export_name) == 0) {
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
    if (rplAddress >= (uint32_t) &gRPLData[0] && rplAddress < (uint32_t) &gRPLData[gModuleData->number_modules]) {
        // Skip if this is our fake RPL
        return 0;
    }
    return real_LiFixupRelocOneRPL(rpl, imports, unknown);
}

DECL_FUNCTION(int32_t, sCheckOne, LOADED_RPL *rpl) {
    auto rplAddress = (uint32_t) rpl;
    if (rplAddress >= (uint32_t) &gRPLData[0] && rplAddress < (uint32_t) &gRPLData[gModuleData->number_modules]) {
        // Skip if this is our fake RPL
        return 0;
    }
    return real_sCheckOne(rpl);
}

function_replacement_data_t dynload_function_replacements[] = {
        REPLACE_FUNCTION_VIA_ADDRESS(__OSDynLoad_InternalAcquire, 0x32029054, 0x101C400 + 0x0cc54),
        REPLACE_FUNCTION_VIA_ADDRESS(LiFindRPLByName, 0x32004BC4, 0x01004bc4),
        REPLACE_FUNCTION_VIA_ADDRESS(LiBinSearchExport, 0x320002f8, 0x010002f8),
        REPLACE_FUNCTION_VIA_ADDRESS(sCheckOne, 0x32007294, 0x01007294),
        REPLACE_FUNCTION_VIA_ADDRESS(LiFixupRelocOneRPL, 0x320059f0, 0x010059f0),
        REPLACE_FUNCTION(OSDynLoad_Acquire, LIBRARY_COREINIT, OSDynLoad_Acquire),
        REPLACE_FUNCTION(OSDynLoad_FindExport, LIBRARY_COREINIT, OSDynLoad_FindExport),
};

uint32_t dynload_function_replacements_size = sizeof(dynload_function_replacements) / sizeof(function_replacement_data_t);