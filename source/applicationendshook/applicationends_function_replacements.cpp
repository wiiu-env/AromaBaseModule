#include "applicationends_function_replacements.h"
#include "globals.h"
#include <coreinit/dynload.h>
#include <coreinit/interrupts.h>
#include <coreinit/messagequeue.h>
#include <coreinit/scheduler.h>
#include <coreinit/title.h>
#include <wums.h>

static uint32_t lastData0 = 0;

void CallHook(wums_hook_type_t type) {
    for (uint32_t i = 0; i < gModuleData->number_modules; i++) {
        auto *curModule = &gModuleData->modules[i];
        for (uint32_t j = 0; j < curModule->number_hook_entries; j++) {
            auto hook_entry = &curModule->hook_entries[j];
            auto hook_type  = (wums_hook_type_t) hook_entry->type;
            if (hook_type == type) {
                if ((type == WUMS_HOOK_INIT_WRAPPER || type == WUMS_HOOK_FINI_WRAPPER) && curModule->skipInitFini) {
                    continue;
                }
                const void *target = (const void *) hook_entry->target;
                ((void (*)())((uint32_t *) target))();
            }
        }
    }
}
void ZombiUFix() {
    /*
     * ZombiU doesn't exit properly. It just calls exit(0) after receiving the
     * request to exit. If exiting takes too long, other threads will run again
     * and crash for some reason. The (hacky) solution is to set the priority
     * of (some) ZombiU threads to something very high (=> low priority).
     */
    if (OSGetTitleID() == 0x000500001010EF00 || // ZombiU EUR
        OSGetTitleID() == 0x000500001011A700 || // ZombiU EUR
        OSGetTitleID() == 0x000500001010DD00 || // ZombiU USA
        OSGetTitleID() == 0x0005000010112300    // ZombiU JPN
    ) {
        auto *curThread = OSGetCurrentThread();
        __OSLockScheduler(curThread);
        int state   = OSDisableInterrupts();
        OSThread *t = *((OSThread **) 0x100567F8);
        while (t) {
            if ((uint32_t) t > (uint32_t) curThread && (uint32_t) t < 0x20000000) {
                t->priority = 0x80;
                OSReport("Set priority to %d for thread %08X (%s) to prevent it from running/crashing\n", t->priority, t, t->name);
            }
            t = t->activeLink.next;
        }

        OSRestoreInterrupts(state);
        __OSUnlockScheduler(curThread);
    }
}

DECL_FUNCTION(uint32_t, OSReceiveMessage, OSMessageQueue *queue, OSMessage *message, uint32_t flags) {
    uint32_t res = real_OSReceiveMessage(queue, message, flags);
    if (queue == OSGetSystemMessageQueue()) {
        if (message != nullptr && res) {
            if (lastData0 != message->args[0]) {
                if (message->args[0] == 0xD1E0D1E0) {
                    ZombiUFix();
                    CallHook(WUMS_HOOK_APPLICATION_REQUESTS_EXIT);
                }
            }
            lastData0 = message->args[0];
        }
    }
    return res;
}

function_replacement_data_t applicationendshook_function_replacements[] = {
        REPLACE_FUNCTION(OSReceiveMessage, LIBRARY_COREINIT, OSReceiveMessage),
};

uint32_t applicationendshook_function_replacements_size = sizeof(applicationendshook_function_replacements) / sizeof(function_replacement_data_t);