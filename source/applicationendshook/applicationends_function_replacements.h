#pragma once

#include <cstdint>
#include <function_patcher/function_patching.h>

#ifdef __cplusplus
extern "C" {
#endif

extern function_replacement_data_t applicationendshook_function_replacements[];


#ifdef __cplusplus
}
#endif


extern uint32_t applicationendshook_function_replacements_size;