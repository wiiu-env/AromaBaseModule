#pragma once

#include <cstdint>
#include <wut.h>

// see https://github.com/decaf-emu/decaf-emu/tree/43366a34e7b55ab9d19b2444aeb0ccd46ac77dea/src/libdecaf/src/cafe/loader
struct LiImportTracking {
    uint32_t numExports;
    struct Export *exports;
    uint32_t tlsModuleIndex;
    struct LOADED_RPL *rpl;
};

struct Export {
    uint32_t value;
    uint32_t name;
};

struct RPLFileInfo_v4_2 {
    char field_0x0[88];
    uint16_t tlsModuleIndex;
    char field_0x5a[6];
};

struct LOADED_RPL {
    char u1[88];
    struct RPLFileInfo_v4_2 *fileInfoBuffer;
    char u2[20];
    uint32_t loadStateFlags;
    uint32_t entrypoint;
    char u3[132];
    uint32_t numFuncExports;
    Export *funcExports;
    uint32_t numDataExports;
    Export *dataExports;
    char u4[12];
};

// https://github.com/decaf-emu/decaf-emu/blob/6feb1be1db3938e6da2d4a65fc0a7a8599fc8dd6/src/libdecaf/src/cafe/libraries/coreinit/coreinit_dynload.cpp#L40
struct RPL_DATA {
    uint32_t handle;
    WUT_UNKNOWN_BYTES(0x94 - 0x4);
};
WUT_CHECK_SIZE(RPL_DATA, 0x94);


#define EXPORT_MASK           0xFFFF0000
#define EXPORT_MAGIC_MASK     0x0000FFFF
#define FUNCTION_EXPORT_MAGIC 0x88660000
#define DATA_EXPORT_MAGIC     0x88550000

#define MODULE_MAGIC_MASK     0xFFFF0000
#define MODULE_ID_MASK        0x0000FFFF
#define MODULE_MAGIC          0x87650000