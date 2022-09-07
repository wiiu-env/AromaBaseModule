#include "patches_replacements.h"
#include "globals.h"
#include "logger.h"
#include "patches.h"
#include <coreinit/filesystem_fsa.h>
#include <coreinit/title.h>

DECL_FUNCTION(FSStatus, FSAddClient, FSClient *client, FSErrorFlag errorMask) {
    auto res = real_FSAddClient(client, errorMask);
    if (res == FS_STATUS_OK) {
        if (((uint32_t) client & 0xF0000000) != gHeapMask) {
            gNonHomebrewFSClientCount++;
        }
    }
    return res;
}

DECL_FUNCTION(FSStatus, FSDelClient, FSClient *client, FSErrorFlag errorMask) {
    auto res = real_FSDelClient(client, errorMask);
    if (res == FS_STATUS_OK) {
        if (((uint32_t) client & 0xF0000000) != gHeapMask) {
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
        titleID == 0x00050000101F7300    // Shantae: Risky's Revenge Director's Cut JPN
    ) {
        DEBUG_FUNCTION_LINE("Fake FSGetClientNum num to %d instead of %d", gNonHomebrewFSClientCount, real_FSGetClientNum());
        return gNonHomebrewFSClientCount;
    }
    return real_FSGetClientNum();
}
function_replacement_data_t patches_function_replacements[] = {
        REPLACE_FUNCTION(FSAddClient, LIBRARY_COREINIT, FSAddClient),
        REPLACE_FUNCTION(FSDelClient, LIBRARY_COREINIT, FSDelClient),
        REPLACE_FUNCTION(FSGetClientNum, LIBRARY_COREINIT, FSGetClientNum),
};

uint32_t patches_function_replacements_size = sizeof(patches_function_replacements) / sizeof(function_replacement_data_t);