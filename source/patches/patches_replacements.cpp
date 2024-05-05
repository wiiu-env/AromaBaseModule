#include "patches_replacements.h"
#include "globals.h"
#include "logger.h"
#include "patches.h"
#include <coreinit/filesystem_fsa.h>
#include <coreinit/title.h>
#include <sysapp/launch.h>

static inline bool IsInHardcodedHomebrewMemoryRegion(void *addr) {
    if ((uint32_t) addr >= 0x00800000 && (uint32_t) addr < 0x01000000) {
        return true;
    }
    return false;
}

DECL_FUNCTION(FSStatus, FSAddClient, FSClient *client, FSErrorFlag errorMask) {
    auto res = real_FSAddClient(client, errorMask);
    if (res == FS_STATUS_OK) {
        if (((uint32_t) client & 0xF0000000) != gHeapMask && !IsInHardcodedHomebrewMemoryRegion(client)) {
            gNonHomebrewFSClientCount++;
        }
    }
    return res;
}

DECL_FUNCTION(FSStatus, FSDelClient, FSClient *client, FSErrorFlag errorMask) {
    auto res = real_FSDelClient(client, errorMask);
    if (res == FS_STATUS_OK) {
        if (((uint32_t) client & 0xF0000000) != gHeapMask && !IsInHardcodedHomebrewMemoryRegion(client)) {
            gNonHomebrewFSClientCount--;
        }
    }
    return res;
}

DECL_FUNCTION(uint32_t, FSGetClientNum) {
    auto titleID = OSGetTitleID();
    // Some games rely on the "correct" FSGetClientNum count and only init their
    // FS system if FSGetClientNum returns 0. Whitelisting games for this hack seems
    // better than breaking other games by accident...
    if (titleID == 0x0005000010129000 || // DuckTales: Remastered USA
        titleID == 0x0005000010129200 || // DuckTales: Remastered EUR
        titleID == 0x00050000101DB200 || // Shantae: Half-Genie Hero USA
        titleID == 0x0005000010203200 || // Shantae: Half-Genie Hero EUR
        titleID == 0x00050000101A6100 || // Shantae: Risky's Revenge Director's Cut USA
        titleID == 0x00050000101A9600 || // Shantae: Risky's Revenge Director's Cut EUR
        titleID == 0x00050000101F7300 || // Shantae: Risky's Revenge Director's Cut JPN
        titleID == 0x000500001014E100 || // Adventure Time: Explore the Dungeon Because I Don't Know! EUR
        titleID == 0x0005000010144000 || // Adventure Time: Explore the Dungeon Because I Don't Know! USA
        titleID == 0x0005000010115D00 || // The Smurfs™ 2 EUR
        titleID == 0x0005000010113300    // The Smurfs™ 2 USA
    ) {
        DEBUG_FUNCTION_LINE_INFO("Fake FSGetClientNum num to %d instead of %d", gNonHomebrewFSClientCount, real_FSGetClientNum());
        return gNonHomebrewFSClientCount;
    }
    return real_FSGetClientNum();
}

DECL_FUNCTION(uint32_t, SYSReturnToCaller, void *args) {
    // Fix jumping back to the System Settings when exiting the System Transfer
    auto curTitleID = OSGetTitleID();
    if (curTitleID == 0x0005001010062000L || curTitleID == 0x0005001010062100L || curTitleID == 0x0005001010062200L) {
        SysAppSettingsArgs set_args{};
        // Jump directly to the Transfer Menu
        set_args.jumpTo = 0xFF;
        _SYSLaunchSettings(&set_args);
        return 0;
    }

    return real_SYSReturnToCaller(args);
}

function_replacement_data_t patches_function_replacements[] = {
        REPLACE_FUNCTION(FSAddClient, LIBRARY_COREINIT, FSAddClient),
        REPLACE_FUNCTION(FSDelClient, LIBRARY_COREINIT, FSDelClient),
        REPLACE_FUNCTION(FSGetClientNum, LIBRARY_COREINIT, FSGetClientNum),
        REPLACE_FUNCTION(SYSReturnToCaller, LIBRARY_SYSAPP, SYSReturnToCaller),
};

uint32_t patches_function_replacements_size = sizeof(patches_function_replacements) / sizeof(function_replacement_data_t);