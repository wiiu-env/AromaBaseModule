#include "sym_function_replacements.h"
#include "globals.h"
#include <cstring>

#pragma GCC push_options
#pragma GCC optimize("O0")

DECL_FUNCTION(uint32_t, SC17_FindClosestSymbol,
              uint32_t addr,
              uint32_t *outDistance,
              char *symbolNameBuffer,
              uint32_t symbolNameBufferLength,
              char *moduleNameBuffer,
              uint32_t moduleNameBufferLength) {
    for (uint32_t i = 0; i < gModuleData->number_modules; i++) {
        auto *module = &gModuleData->modules[i];
        if (addr < module->startAddress || addr >= module->endAddress) {
            continue;
        }

        strncpy(moduleNameBuffer, module->module_export_name, moduleNameBufferLength);

        if (module->function_symbol_entries != nullptr && module->number_function_symbols > 1) {
            for (uint32_t j = 0; j < module->number_function_symbols - 1; j++) {
                auto symbolData     = &module->function_symbol_entries[j];
                auto symbolDataNext = &module->function_symbol_entries[j + 1];
                if (j == module->number_function_symbols - 2 || (addr >= (uint32_t) symbolData->address && addr < (uint32_t) symbolDataNext->address)) {
                    strncpy(symbolNameBuffer, symbolData->name, moduleNameBufferLength);
                    if (outDistance) {
                        *outDistance = addr - (uint32_t) symbolData->address;
                    }
                    return 0;
                }
            }
        }

        strncpy(symbolNameBuffer, ".text", symbolNameBufferLength);

        if (outDistance) {
            *outDistance = addr - (uint32_t) module->startAddress;
        }

        return 0;
    }
    return real_SC17_FindClosestSymbol(addr, outDistance, symbolNameBuffer, symbolNameBufferLength, moduleNameBuffer, moduleNameBufferLength);
}

DECL_FUNCTION(uint32_t, KiGetAppSymbolName, uint32_t addr, char *buffer, int32_t bufSize) {
    for (uint32_t i = 0; i < gModuleData->number_modules; i++) {
        auto *module = &gModuleData->modules[i];
        if (addr < module->startAddress || addr >= module->endAddress) {
            continue;
        }

        auto moduleNameLen        = strlen(module->module_export_name);
        int32_t spaceLeftInBuffer = (int32_t) bufSize - (int32_t) moduleNameLen - 1;
        if (spaceLeftInBuffer < 0) {
            spaceLeftInBuffer = 0;
        }
        strncpy(buffer, module->module_export_name, bufSize);

        if (module->function_symbol_entries != nullptr && module->number_function_symbols > 1) {
            for (uint32_t j = 0; j < module->number_function_symbols - 1; j++) {
                auto symbolData     = &module->function_symbol_entries[j];
                auto symbolDataNext = &module->function_symbol_entries[j + 1];
                if (j == module->number_function_symbols - 2 || (addr >= (uint32_t) symbolData->address && addr < (uint32_t) symbolDataNext->address)) {
                    if (spaceLeftInBuffer > 2) {
                        buffer[moduleNameLen]     = '|';
                        buffer[moduleNameLen + 1] = '\0';
                        strncpy(buffer + moduleNameLen + 1, symbolData->name, spaceLeftInBuffer - 1);
                    }
                    return (uint32_t) symbolData->address;
                }
            }
        }
        return addr;
    }

    return real_KiGetAppSymbolName(addr, buffer, bufSize);
}
#pragma GCC pop_options

function_replacement_data_t symbolname_function_replacements[] = {
        REPLACE_FUNCTION_VIA_ADDRESS(SC17_FindClosestSymbol, 0xfff10218, 0xfff10218),
        REPLACE_FUNCTION_VIA_ADDRESS(KiGetAppSymbolName, 0xfff0e3a0, 0xfff0e3a0),
};

uint32_t symbolname_function_replacements_size = sizeof(symbolname_function_replacements) / sizeof(function_replacement_data_t);