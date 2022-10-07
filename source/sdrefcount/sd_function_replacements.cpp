#include "sd_function_replacements.h"
#include "applicationendshook/applicationends_function_replacements.h"
#include "globals.h"
#include "logger.h"
#include <coreinit/dynload.h>
#include <coreinit/filesystem_fsa.h>
#include <coreinit/interrupts.h>
#include <coreinit/scheduler.h>
#include <coreinit/title.h>
#include <string_view>

uint64_t NWFPatchTitleList[] = {
        0x0005000010102900L, // Demaecan JPN
        0x0005000010102A00L, // YNN JPN
        0x0005000010102D00L, // Hulu USA
        0x0005000010102E00L, // Amazon Instant Video USA
        0x0005000010102F00L, // Amazon / LOVEFiLM EUR
        0x0005000010104C00L, // Hulu JPN
        0x0005000010105700L, // YouTube ALL
        0x000500001012D300L, // Wii Street U powered by Google JPN
        0x0005000010132000L, // Wii Street U powered by Google USA
        0x0005000010132100L, // Wii Street U powered by Google EUR
        0x0005000010136100L, // BANDAI CHANNEL JPN
        0x0005000010149300L, // Mario vs. Donkey Kong Tipping Stars USA
        0x0005000010154500L, // Togabito JPN
        0x0005000010154800L, // BBC iPlayer EUR
        0x0005000010157D00L, // GAIABREAKER JPN
        0x000500001016E200L, // XType Plus EUR
        0x000500001016E500L, // BLOK DROP U USA
        0x000500001016E600L, // ZaciSa's Last Stand USA
        0x0005000010170400L, // XType Plus USA
        0x0005000010172A00L, // Crunchyroll USA
        0x0005000010172B00L, // Crunchyroll EUR
        0x0005000010173000L, // Nintendo Game Seminar 2013 Student Works JPN
        0x0005000010173100L, // I've Got to Run! USA
        0x0005000010175700L, // BLOK DROP U EUR
        0x0005000010175F00L, // Death Match Love Comedy JPN
        0x0005000010176400L, // Pixel Paint USA
        0x0005000010178E00L, // Mario vs. Donkey Kong Tipping Stars EUR
        0x0005000010179200L, // Mario vs. Donkey Kong Tipping Stars JPN
        0x0005000010179300L, // Internal Invasion USA
        0x0005000010179500L, // Plenty of Fishies USA
        0x000500001017A800L, // GAIABREAKER USA
        0x000500001017A900L, // SUPER ROBO MOUSE USA
        0x000500001017AB00L, // GAIABREAKER EUR
        0x000500001017DB00L, // Dolphin Up USA
        0x000500001017E200L, // 3Souls EUR
        0x000500001017E700L, // 3Souls USA
        0x000500001017EB00L, // SDK Paint USA
        0x000500001017EC00L, // Guac' a Mole EUR
        0x0005000010182E00L, // GetClose USA
        0x0005000010183100L, // Snake Den USA
        0x0005000010183400L, // U Host USA
        0x0005000010185000L, // ブタメダル JPN
        0x0005000010185900L, // GolT Stadium EUR
        0x000500001018A400L, // POKER DICE SOLITAIRE FUTURE USA
        0x000500001018D700L, // SHUT THE BOX USA
        0x000500001018EE00L, // Don't Touch Anything Red USA
        0x000500001018F900L, // TOSS N GO USA
        0x000500001018FE00L, // Sense by Play.me EUR
        0x0005000010190400L, // Red Riding Hood USA
        0x0005000010193900L, // Flapp & Zegeta USA
        0x0005000010193D00L, // Tiny Galaxy USA
        0x0005000010193E00L, // Twisted Fusion USA
        0x0005000010195F00L, // Arrow Time U USA
        0x0005000010198700L, // Watchup USA
        0x000500001019A100L, // Shapes of Gray USA
        0x000500001019A300L, // SPIKEY WALLS USA
        0x000500001019A700L, // BLOK DROP X TWISTED FUSION USA
        0x000500001019A800L, // Citadale The Legends Trilogy USA
        0x000500001019AD00L, // Elliot Quest USA
        0x000500001019B900L, // SUPER ROBO MOUSE EUR
        0x000500001019BA00L, // BLOK DROP X TWISTED FUSION EUR
        0x00050000101A2400L, // Tested with robots ! EUR
        0x00050000101A2E00L, // XType+ JPN
        0x00050000101A3E00L, // Defense Dome USA
        0x00050000101A4400L, // Toon Tanks USA
        0x00050000101A9100L, // Rakuten TV JPN
        0x00050000101A9F00L, // Dot Arcade USA
        0x00050000101AA300L, // SDK Spriter USA
        0x00050000101AAC00L, // Red Riding Hood EUR
        0x00050000101ABB00L, // COLOR BOMBS USA
        0x00050000101ACB00L, // psyscrolr USA
        0x00050000101AEB00L, // SDK Paint EUR
        0x00050000101AEF00L, // Midnight USA
        0x00050000101AF400L, // PixlCross USA
        0x00050000101B4F00L, // Asteroid Quarry USA
        0x00050000101B5000L, // Drop It: Block Paradise! USA
        0x00050000101B5E00L, // SPLASHY DUCK EUR
        0x00050000101B5F00L, // TABLETOP GALLERY EUR
        0x00050000101B8400L, // Mortar Melon USA
        0x00050000101B9600L, // Super Destronaut USA
        0x00050000101B9700L, // SDK Spriter EUR
        0x00050000101BA100L, // Best way to motivate kids ManabiGet! JPN
        0x00050000101BA700L, // PENTAPUZZLE USA
        0x00050000101BB300L, // ZaciSa: Defense of the Crayon Dimension! EUR
        0x00050000101BB800L, // Elliot Quest EUR
        0x00050000101BC000L, // ドットペイント JPN
        0x00050000101BC400L, // Plenty of Fishies EUR
        0x00050000101BC500L, // Mortar Melon EUR
        0x00050000101BCA00L, // Napster EUR
        0x00050000101BCB00L, // Rhapsody USA
        0x00050000101BCF00L, // Word Search by POWGI USA
        0x00050000101C3900L, // Word Search by POWGI EUR
        0x00050000101C3C00L, // PictoParty EUR
        0x00050000101C4A00L, // Word Party EUR
        0x00050000101C5500L, // Word Party USA
        0x00050000101C5900L, // サイコロの森 3-in-1L, // JPN
        0x00050000101C6100L, // Mini Mario & Friends amiibo Challenge JPN
        0x00050000101C6200L, // Mini Mario & Friends amiibo Challenge USA
        0x00050000101C6300L, // Mini Mario & Friends amiibo Challenge EUR
        0x00050000101C6900L, // TABLETOP GALLERY USA
        0x00050000101C6B00L, // PEG SOLITAIRE USA
        0x00050000101C7200L, // Placards USA
        0x00050000101C7300L, // The Stonecutter USA
        0x00050000101C8E00L, // Drop It: Block Paradise! EUR
        0x00050000101CA300L, // Neon Battle USA
        0x00050000101CF900L, // Word Puzzles by POWGI EUR
        0x00050000101CFA00L, // Word Puzzles by POWGI USA
        0x00050000101CFD00L, // Near Earth Objects USA
        0x00050000101CFF00L, // Job the Leprechaun USA
        0x00050000101D1700L, // Booty Diver USA
        0x00050000101D1A00L, // Aenigma Os USA
        0x00050000101D2800L, // DON'T CRASH USA
        0x00050000101D2900L, // PIXEL SLIME U USA
        0x00050000101D2A00L, // Land it Rocket USA
        0x00050000101D6200L, // Test Your Mind USA
        0x00050000101D6700L, // Star Sky EUR
        0x00050000101D7300L, // Molly Maggot USA
        0x00050000101D7700L, // Explody Bomb USA
        0x00050000101D7800L, // Hold Your Fire USA
        0x00050000101D8A00L, // Star Sky USA
        0x00050000101D8B00L, // ヒラメキパズル テトグラム JPN
        0x00050000101D8C00L, // ブルームーン JPN
        0x00050000101D9C00L, // Funky Physics USA
        0x00050000101D9F00L, // Job the Leprechaun EUR
        0x00050000101DB100L, // PictoParty USA
        0x00050000101DD500L, // Elliot Quest JPN
        0x00050000101DEE00L, // AVOIDER USA
        0x00050000101DFA00L, // Draw 2 Survive USA
        0x00050000101DFD00L, // Snowball USA
        0x00050000101E2E00L, // Christmas Adventure of Rocket P. USA
        0x00050000101E3200L, // Jewel Quest USA
        0x00050000101E3300L, // Olympia Rising EUR
        0x00050000101E3400L, // Olympia Rising USA
        0x00050000101E5500L, // Escape From Flare Industries USA
        0x00050000101E7800L, // SKEASY USA
        0x00050000101E7900L, // SPLASHY DUCK USA
        0x00050000101E7C00L, // TAP TAP ARCADE EUR
        0x00050000101E7D00L, // TAP TAP ARCADE USA
        0x00050000101E7E00L, // TOUCH SELECTIONS EUR
        0x00050000101E7F00L, // TOUCH SELECTIONS USA
        0x00050000101E8800L, // COLOR BOMBS EUR
        0x00050000101E9700L, // U Host EUR
        0x00050000101EA900L, // Armored ACORNs Action Squirrel Squad USA
        0x00050000101EC600L, // Defense Dome EUR
        0x00050000101EF400L, // Demonic Karma Summoner USA
        0x00050000101F2700L, // Star Sky 2 EUR
        0x00050000101F3E00L, // Star Sky 2 USA
        0x00050000101F5900L, // PENTAPUZZLE EUR
        0x00050000101F5F00L, // Hot Rod Racer USA
        0x00050000101F6200L, // Adventure Party Cats and Caverns USA
        0x00050000101F6600L, // Gravity+ USA
        0x00050000101F6A00L, // Jewel Quest EUR
        0x00050000101F6B00L, // PIXEL SLIME U EUR
        0x00050000101F7400L, // Hot Rod Racer EUR
        0x00050000101F7500L, // Midnight 2 USA
        0x00050000101F7C00L, // TAP TAP ARCADE 2 USA
        0x00050000101F8100L, // Gravity+ EUR
        0x00050000101F8400L, // JACKPOT 777 EUR
        0x00050000101F8D00L, // TAP TAP ARCADE 2 EUR
        0x00050000101F9000L, // Citadale USA
        0x00050000101F9200L, // JACKPOT 777 USA
        0x00050000101F9400L, // SD2: Go Duck Yourself USA
        0x00050000101F9900L, // Aenigma Os EUR
        0x00050000101F9A00L, // Citadale EUR
        0x00050000101FAB00L, // Collateral Thinking USA
        0x00050000101FAC00L, // INVANOID USA
        0x00050000101FAD00L, // SHOOTY SPACE USA
        0x00050000101FAF00L, // Midnight 2 EUR
        0x00050000101FB000L, // SD2: Go Duck Yourself EUR
        0x00050000101FB400L, // Shadow Archer USA
        0x00050000101FC000L, // BLOC USA
        0x00050000101FCE00L, // Word Logic by POWGI USA
        0x00050000101FD200L, // SUPER ROBO MOUSE JPN
        0x00050000101FD300L, // BLOK DROP U JPN
        0x00050000101FE600L, // Amazon Video JPN
        0x00050000101FEC00L, // Toon Tanks EUR
        0x00050000101FF900L, // Test Your Mind EUR
        0x00050000101FFA00L, // Word Logic by POWGI EUR
        0x0005000010200200L, // Collateral Thinking EUR
        0x0005000010200700L, // Twisted Fusion EUR
        0x0005000010200E00L, // PANDA LOVE USA
        0x0005000010200F00L, // PANDA LOVE EUR
        0x0005000010201500L, // TITANS TOWER USA
        0x0005000010201600L, // TITANS TOWER EUR
        0x0005000010201800L, // SHOOTY SPACE EUR
        0x0005000010202100L, // Overworld Defender Remix USA
        0x0005000010202500L, // Alice in Wonderland USA
        0x0005000010202600L, // Laser Blaster USA
        0x0005000010202700L, // Rorrim USA
        0x0005000010202900L, // Amazon Video BETA JPN
        0x0005000010202A00L, // Amazon Video USA
        0x0005000010202B00L, // Laser Blaster EUR
        0x0005000010203600L, // Rorrim EUR
        0x0005000010203900L, // Alice in Wonderland EUR
        0x0005000010203D00L, // Space Hunted: The Lost Levels USA
        0x0005000010207600L, // Space Hunted USA
        0x0005000010207700L, // Y.A.S.G USA
        0x0005000010207B00L, // Shadow Archer EUR
        0x0005000010207C00L, // A.C.E. - Alien Cleanup Elite USA
        0x0005000010208E00L, // A.C.E. EUR
        0x0005000010209D00L, // Sudoku Party EUR
        0x0005000010209E00L, // Sudoku Party USA
        0x000500001020A100L, // Star Sky 2 JPN
        0x000500001020B800L, // Scribble EUR
        0x000500001020BB00L, // Scribble USA
        0x000500001020C700L, // BLOC EUR
        0x000500001020D100L, // Pic-a-Pix Colour EUR
        0x000500001020D700L, // Pic-a-Pix Color USA
        0x000500001020F500L, // Absolutely Unstoppable MineRun EUR
        0x000500001020FD00L, // Absolutely Unstoppable MineRun USA
        0x0005000010210300L, // Citadale The Legends Trilogy EUR
        0x0005000010210400L, // Space Hunted EUR
        0x0005000010210500L, // Y.A.S.G EUR
        0x0005000010210700L, // The Stonecutter EUR
        0x0005000010211000L, // Coqui The Game USA
        0x0005000010211400L, // Daikon Set USA
        0x0005000010211E00L, // A Drawing's Journey EUR
        0x0005000010212500L, // Armored ACORNs Action Squirrel Squad EUR
        0x0005000010213500L, // Super Ultra Star Shooter EUR
        0x0005000010213B00L, // Cubeshift USA
        0x0005000010213C00L, // Super Ultra Star Shooter USA
        0x0005000010215F00L, // Spellcaster's Assistant EUR
        0x0005000010216100L, // Spellcaster's Assistant USA
        0x0005000010216D00L, // Shadow Archery USA
        0x0005000010216E00L, // Shadow Archery EUR
        0x0005000010217C00L, // Sinister Assistant EUR
        0x0005000010217F00L, // Sinister Assistant USA
        0x0005000010219C00L, // Space Hunted: The Lost Levels EUR
        0x000500001021B200L, // Captain U USA
        0x000500001021C000L, // Seasonal Assistant USA
        0x000500001021C100L, // Seasonal Assistant EUR
        0x000500001021C600L, // Captain U EUR
        0x000500001021C900L, // Reversi 32 EUR
        0x000500021016E601L, // ZaciSa's Last Stand Demo USA
        0x000500021017E201L, // 3Souls (Demo) EUR
        0x000500021017E701L, // 3Souls (Demo) USA
        0x000500021017EB01L, // SDK Paint Demo USA
        0x00050002101A2E01L, // XType+ (体験版） JPN
        0x00050002101AA301L, // SDK Spriter DEMO USA
        0x00050002101AEB01L, // SDK Paint Demo EUR
        0x00050002101B9701L, // SDK Spriter DEMO EUR
        0x00050002101BB301L, // ZaciSa: Defense of the Crayon Dimension! Demo EUR
        0x00050002101BCF01L, // Word Search by POWGI (Demo) USA
        0x00050002101C3901L, // Word Search by POWGI (Demo) EUR
        0x00050002101C4A01L, // Word Party (Demo) EUR
        0x00050002101C5501L, // Word Party (Demo) USA
        0x00050002101C5901L, // サイコロの森 3-in-1（体験版） JPN
        0x00050002101CF901L, // Word Puzzles by POWGI (Demo) EUR
        0x00050002101CFA01L, // Word Puzzles by POWGI (Demo) USA
        0x00050002101D8B01L, // ヒラメキパズル テトグラム（体験版) JPN
        0x00050002101FCE01L, // Word Logic by POWGI (Demo) USA
        0x00050002101FFA01L, // Word Logic by POWGI (Demo) EUR
        0x000500021020D101L, // Pic-a-Pix Colour (Demo) EUR
        0x000500021020D701L, // Pic-a-Pix Color (Demo) USA
};

void NWF_Fix() {
    auto titleID = OSGetTitleID();
    bool found   = false;
    for (auto &tid : NWFPatchTitleList) {
        if (tid == titleID) {
            DEBUG_FUNCTION_LINE_WARN("Title using \"LibPlatform::Input::GetInstance()\" detected!");
            found = true;
            break;
        }
    }
    if (!found) {
        return;
    }
    // Some games using the NWF (like Dot Arcade) keep a thread called "PlatformInputAppStateListenerThread" running,
    // which will cause a DSI exception if the title is not closing fast enough.
    auto *curThread = OSGetCurrentThread();
    __OSLockScheduler(curThread);
    int state   = OSDisableInterrupts();
    OSThread *t = *((OSThread **) 0x100567F8);
    while (t) {
        if (std::string_view(t->name) == "PlatformInputAppStateListenerThread") {
            t->priority = 0x80;
            OSReport("Set priority to %d for thread \"%s\" (%08X) to prevent it from running/crashing\n", t->priority, t->name, t);
        }
        t = t->activeLink.next;
    }
    OSRestoreInterrupts(state);
    __OSUnlockScheduler(curThread);
}

DECL_FUNCTION(void, __PPCExit, uint32_t u1) {
    NWF_Fix();

    CallHook(WUMS_HOOK_APPLICATION_ENDS);
    CallHook(WUMS_HOOK_FINI_WUT_SOCKETS);
    CallHook(WUMS_HOOK_FINI_WUT_DEVOPTAB);
    if (gSDMountRefCount > 0) {
        FSAInit();
        auto client = FSAAddClient(nullptr);
        if (client) {
            FSAUnmount(client, "/vol/external01/", FSA_UNMOUNT_FLAG_BIND_MOUNT);
        }
        gSDMountRefCount = 0;
    }

    if (gModuleData->number_acquired_rpls > 0) {
        DEBUG_FUNCTION_LINE_VERBOSE("Release RPLs acquired by modules");
        for (uint32_t i = 0; i < gModuleData->number_acquired_rpls; i++) {
            DEBUG_FUNCTION_LINE_VERBOSE("OSDynLoad_Release(0x%08X)", gModuleData->acquired_rpls[i]);
            OSDynLoad_Release((void *) gModuleData->acquired_rpls[i]);
        }
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