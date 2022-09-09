#pragma once

#include <cstdint>
#include <function_patcher/function_patching.h>
#include <wums/hooks.h>

#ifdef __cplusplus
extern "C" {
#endif

void CallHook(wums_hook_type_t type);

extern function_replacement_data_t applicationendshook_function_replacements[];


#ifdef __cplusplus
}
#endif


extern uint32_t applicationendshook_function_replacements_size;