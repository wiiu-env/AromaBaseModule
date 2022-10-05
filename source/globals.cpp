#include "globals.h"

module_information_t *gModuleData __attribute__((section(".data"))) = NULL;
int32_t gSDMountRefCount __attribute__((section(".data")))          = 0;
LOADED_RPL *gLoadedRPLData __attribute__((section(".data")))        = nullptr;
RPL_DATA *gRPLData __attribute__((section(".data")))                = nullptr;