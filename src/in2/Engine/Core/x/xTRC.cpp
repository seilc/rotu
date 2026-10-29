#include "xTRC.h"

#include "xDebug.h"
#include "xFont.h"
#include "xPad.h"
#include "xTextAsset.h"
#include "iDraw.h"
#include "zGameState.h"
#include "zGlobals.h"
#include "zMenu.h"
#include "zPad.h"
#include "zPlayer.h"
#include "zVar.h"
#include "zMrIncrediblePlayer.h"
#include "zFrozonePlayer.h"
#include "zScene.h"

#include <string.h>
#include "xSpacePartitionTree.h"

namespace
{
_tagTRCPadInfo gTrcPad[k_MAX_TRC_PADS];
_tagTRCDiskInfo gTrcDisk;
const char* k_CONTROLLER_MISSING_MESSAGE;
const char* k_CONTROLLER_INVALID_MESSAGE;

inline void xTRCSwapPads(_tagTRCPadInfo& pad0, _tagTRCPadInfo& pad1)
{
    _tagTRCPadInfo temppad;
    temppad.id = pad0.id;
    temppad.state = pad0.state;
    temppad.padstate = pad0.padstate;
    pad0.id = pad1.id;
    pad0.state = pad1.state;
    pad0.padstate = pad1.padstate;
    pad1.id = temppad.id;
    pad1.state = temppad.state;
    pad1.padstate = temppad.padstate;
}
}

bool xTRCMemCardCheck()
{
    return false;
}

_tagTRCPadInfo& xTRCPadGetNextAvailablePad()
{
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++) {
        if (gTrcPad[i].state == TRC_Unknown) {
            gTrcPad[i].state = TRC_PadInserted;
            gTrcPad[i].padstate = TRC_PadStatePresent;
            return gTrcPad[i];
        }
    }
    xFAIL_MSG("No pads available!");
    return gTrcPad[0];
}

S32 xTRCPadGetArrayIdx(S32 port)
{
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++) {
        if (port == gTrcPad[i].id) {
            return i;
        }
    }
    xFAIL_MSG("No pad id match, return -1 !");
    return -1;
}

void xTRCPadRemove(S32 port)
{
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++) {
        if (port == gTrcPad[i].id) {
            if (gTrcPad[i].state == TRC_PadActive || gTrcPad[i].padstate == TRC_PadStateMissingDuringGameplay) {
                gTrcPad[i].state = TRC_PadMissing;
                gTrcPad[i].padstate = TRC_PadStateMissingDuringGameplay;
                xTRCSwapPads(gTrcPad[0], gTrcPad[i]);
            } else {
                gTrcPad[i].id = -1;
                gTrcPad[i].state = TRC_Unknown;
                gTrcPad[i].padstate = TRC_PadStateUnknown;
            }
            return;
        }
    }
}

void xTRCPadRestoreAll()
{
    for (S32 i = 0; i < k_XPAD_MAX; i++) {
        xPad* pad = &mPad[i];
        if (pad->state == ePad_Enabled) {
            xASSERT(pad->port != -1);
            if (xTRCPadGetPadStatePort(pad->port) == TRC_PadStateMissingDuringGameplay) {
                xTRCPadSetPressed(pad->port, 1, false);
                xTRCPadSetStatePadPort(pad->port, TRC_PadInserted);
                S32 idx = xTRCPadGetArrayIdx(pad->port);
                xTRCUpdate(idx);
            }
        }
    }
}

void xTRCPadSetState(S32 idx, TRCState state)
{
    xASSERT(idx >= 0 && idx < k_MAX_TRC_PADS);
    gTrcPad[idx].state = state;
}

S32 xTRCPadCount(TRCState state)
{
    S32 counter = 0, startidx = 0;
    while (xTRCPadFindFirst(startidx, state)) {
        counter++; startidx++;
    }
    return counter;
}

S32 xTRCPadCount(TRCPadState padstate)
{
    S32 counter = 0, startidx = 0;
    while (xTRCPadFindFirstPadState(startidx, padstate)) {
        counter++; startidx++;
    }
    return counter;
}

S32 xTRCPadCountWereActiveTotal()
{
    S32 activecount = xTRCPadCount(TRC_PadActive);
    S32 missingcount = xTRCPadCount(TRC_PadMissing);
    S32 padstatecount = xTRCPadCount(TRC_PadStateMissingDuringGameplay);
    if (padstatecount == 0 && missingcount == 0 && activecount == 0) {
        return 0;
    } else if (activecount == 2 || (activecount == 1 && (missingcount == 1 || padstatecount == 1)) || missingcount == 2 || padstatecount == 2) {
        return 2;
    }
    return 1;
}

void xTRCPadSetStatePadPort(S32 port, TRCState state)
{
    xASSERT(port >= 0 && port < k_MAX_TRC_PADS);
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++)
        if (port == gTrcPad[i].id)
            gTrcPad[i].state = state;
}

void xTRCPadSetPadStatePort(S32 port, TRCPadState state)
{
    xASSERT(port >= 0 && port < k_MAX_TRC_PADS);
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++)
        if (port == gTrcPad[i].id)
            gTrcPad[i].padstate = state;
}

bool xTRCPadFindFirst(S32& idx, TRCState state)
{
    xASSERT(idx > -1);
    S32 i = -1;
    while (idx < k_MAX_TRC_PADS && state != gTrcPad[idx].state) {
        i = ++idx;
    }
    xASSERT(i >= -1 && i <= k_MAX_TRC_PADS);
    if (i == k_MAX_TRC_PADS || idx >= k_MAX_TRC_PADS) {
        idx = -1;
        return false;
    }
    return true;
}

bool xTRCPadFindFirstPadState(S32& idx, TRCPadState state)
{
    xASSERT(idx > -1);
    S32 i = -1;
    while (idx < k_MAX_TRC_PADS && state != gTrcPad[idx].padstate) {
        i = ++idx;
    }
    xASSERT(i >= -1 && i <= k_MAX_TRC_PADS);
    if (i == k_MAX_TRC_PADS || idx >= k_MAX_TRC_PADS) {
        idx = -1;
        return false;
    }
    return true;
}

bool xTRCPadIsIdx(S32 idx, TRCState state)
{
    xASSERT(idx >= 0 && idx < k_MAX_TRC_PADS);
    return state == gTrcPad[idx].state ? true : false;
}

S32 xTRCPadGetPadPort(S32 idx)
{
    xASSERT(idx >= 0 && idx < k_MAX_TRC_PADS);
    return gTrcPad[idx].id;
}

TRCState xTRCPadGetStateIdx(S32 idx)
{
    xASSERT(idx >= 0 && idx < k_MAX_TRC_PADS);
    return gTrcPad[idx].state;
}

TRCState xTRCPadGetStatePort(S32 port)
{
    xASSERT(port >= 0 && port < k_MAX_TRC_PADS);
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++) {
        if (port == gTrcPad[i].id) {
            return gTrcPad[i].state;
        }
    }
    return TRC_Unknown;
}

TRCPadState xTRCPadGetPadStatePort(S32 port)
{
    xASSERT(port >= 0 && port < k_MAX_TRC_PADS);
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++) {
        if (port == gTrcPad[i].id) {
            return gTrcPad[i].padstate;
        }
    }
    return TRC_PadStateUnknown;
}

bool xTRCPadIsPort(S32 port, TRCState state)
{
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++) {
        if (port == gTrcPad[i].id) {
            if (state == gTrcPad[i].state) {
                return true;
            } else {
                return false;
            }
        }
    }
    if (state == TRC_Unknown) {
        return true;
    }
    return false;
}

bool xTRCPadGetPadPort(S32& idx, TRCState state)
{
    S32 i = 0;
    while (i < k_MAX_TRC_PADS && state != gTrcPad[i].state) {
        idx = gTrcPad[i++].id;
    }
    xASSERT(idx >= -1 && idx < k_MAX_TRC_PADS);
    if (i == k_MAX_TRC_PADS) {
        return false;
    }
    idx = gTrcPad[i].id;
    return true;
}

bool xTRCPadPortWasActive(S32 port)
{
    return xTRCPadIsPort(port, TRC_PadActive) || xTRCPadIsPort(port, TRC_PadPreInserted) || xTRCPadGetPadStatePort(port) == TRC_PadStateMissingDuringGameplay;
}

namespace
{
bool xTRCIsNextState(S32 i, U32 state)
{
    return (TRCState)state != gTrcPad[i].state && (TRCState)state == gTrcPad[i + 1].state;
}

bool xTRCIsNextPadState(S32 i, U32 state)
{
    return (TRCPadState)state != gTrcPad[i].padstate && (TRCPadState)state == gTrcPad[i + 1].padstate;
}

bool xTRCBabbleSort(bool (*funcShake)(S32, U32), U32 state)
{
    bool babble = false;
    S32 counter = 0;
    do {
        babble = false;
        for (S32 i = 0; i < k_MAX_TRC_PADS - 1; i++) {
            if (funcShake(i, state)) {
                xTRCSwapPads(gTrcPad[i], gTrcPad[i + 1]);
                babble = true;
                counter++;
            }
        }
    } while (babble);
    return counter > 0;
}
}

void xTRCPadReOrder()
{
    xTRCBabbleSort(xTRCIsNextState, TRC_PadActive);
    xTRCBabbleSort(xTRCIsNextState, TRC_PadPreInserted);
    if (!xTRCBabbleSort(xTRCIsNextPadState, TRC_PadStateMissingDuringGameplay)) {
        xTRCBabbleSort(xTRCIsNextState, TRC_PadMissing);
    }
}

void xTRCSetCharactersControl(S32 i)
{
    S32 mriPad = zPlayerMRIGetLastUsedPad();
    S32 froPad = zPlayerFROGetLastUsedPad();
    if (mriPad == -1 && froPad == -1) {
        if (globals.sceneCur->sceneID != 'MNUS') {
            if (xTRCPadCountWereActiveTotal() == 1) {
                if (xglobals->players.GetMrI()->Get_humanPlayer()) {
                    zPlayerMRICreatePartAtPort(-1);
                } else {
                    zPlayerFROCreatePartAtPort(-1);
                }
            } else if (xTRCPadCountWereActiveTotal() == 2) {
                zPlayerMRICreatePartAtPort(-1);
                zPlayerFROCreatePartAtPort(-1);
            }
        }
    } else {
        if (xglobals->players.GetMrI()->Get_humanPlayer()) {
            if (!xglobals->players.GetMrI()->GetPad() || xglobals->players.GetMrI()->GetPad()->GetPadPort() == -1) {
                if (mriPad == gTrcPad[i].id) {
                    zPlayerMRICreatePartAtPort(gTrcPad[i].id);
                } else if (mriPad == -1) {
                    zPlayerMRICreatePartAtPort(-1);
                }
            }
        }
        if (xglobals->players.GetFrozone()->Get_humanPlayer()) {
            if (!xglobals->players.GetFrozone()->GetPad() || xglobals->players.GetFrozone()->GetPad()->GetPadPort() == -1) {
                if (froPad == gTrcPad[i].id) {
                    zPlayerFROCreatePartAtPort(gTrcPad[i].id);
                } else if (froPad == -1) {
                    zPlayerFROCreatePartAtPort(-1);
                }
            }
        }
    }
}

S32 xTRCPadGetPort_Player1()
{
    for (S32 i = 0; i < xglobals->players.size(); i++) {
        if (xglobals->players[i]->IsUsingLeftHUD()) {
            zPad* pad = xglobals->players[i]->GetPad();
            if (pad) {
                return pad->GetPadPort();
            }
            return -1;
        }
    }
    return -1;
}

S32 xTRCPadGetPort_Player2()
{
    for (S32 i = 0; i < xglobals->players.size(); i++) {
        if (!xglobals->players[i]->IsUsingLeftHUD()) {
            zPad* pad = xglobals->players[i]->GetPad();
            if (pad) {
                return pad->GetPadPort();
            }
            return -1;
        }
    }
    return -1;
}

void xTRCUpdate(S32 i)
{
    for (i = 0; i < k_MAX_TRC_PADS; i++) {
        switch (gTrcPad[i].state) {
        case TRC_PadPreInserted:
            switch (gTrcPad[i].padstate) {
            case TRC_PadStateMissingDuringGameplay:
                if ((mPad[gTrcPad[i].id].released & 0x1) || (!mPad[gTrcPad[i].id].pressed & 0x1)) {
                    gTrcPad[i].state = TRC_PadInserted;
                    mPad[gTrcPad[i].id].pressed = 0;
                    mPad[gTrcPad[i].id].released = 0;
                    xTRCPadReOrder();
                }
                break;
            }
            break;
        case TRC_PadInserted:
            switch (gTrcPad[i].padstate) {
            case TRC_PadStateMissingDuringGameplay:
                gTrcPad[i].state = TRC_PadActive;
                gTrcPad[i].padstate = TRC_PadStatePresent;
                mPad[gTrcPad[i].id].pressed = 0;
                mPad[gTrcPad[i].id].released = 0;
                xTRCSetCharactersControl(i);
                S32 padPortP1 = xTRCPadGetPort_Player1();
                S32 padPortP2 = xTRCPadGetPort_Player2();
                if (xTRCPadCountWereActiveTotal() == 1) {
                    if (globals.option_vibration_p1)
                        xPadRumbleEnable(gTrcPad[i].id, 1);
                    else
                        xPadRumbleEnable(gTrcPad[i].id, 0);
                } else {
                    if (padPortP1 == gTrcPad[i].id) {
                        if (globals.option_vibration_p1)
                            xPadRumbleEnable(gTrcPad[i].id, 1);
                        else
                            xPadRumbleEnable(gTrcPad[i].id, 0);
                    }
                    if (padPortP2 == gTrcPad[i].id) {
                        if (globals.option_vibration_p2)
                            xPadRumbleEnable(gTrcPad[i].id, 1);
                        else
                            xPadRumbleEnable(gTrcPad[i].id, 0);
                    }
                }
                xTRCPadReOrder();
                S32 startidx = 0;
                if (!xTRCPadFindFirst(startidx, TRC_PadMissing)) {
                    startidx = 0;
                    if (!xTRCPadFindFirstPadState(startidx, TRC_PadStateMissingDuringGameplay)) {
                        xTRCReset();
                    }
                }
                break;
            case TRC_PadStatePresent:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void xTRCPadInsert(S32 i)
{
    if (xTRCPadIsPort(i, TRC_PadMissing)) {
        xTRCPadSetStatePadPort(i, TRC_PadPreInserted);
    }
    else if (xTRCPadIsPort(i, TRC_Unknown)) {
        _tagTRCPadInfo& trcpad = xTRCPadGetNextAvailablePad();
        trcpad.id = i;
    }
}

void xTRCInit()
{
    memset(gTrcPad, 0, sizeof(gTrcPad));
    for (S32 i = 0; i < k_MAX_TRC_PADS; i++) {
        gTrcPad[i].id = -1;
        gTrcPad[i].state = TRC_Unknown;
        gTrcPad[i].padstate = TRC_PadStateUnknown;
    }
    memset(&gTrcDisk, 0, sizeof(gTrcDisk));
}

void xTRCInitMessages()
{
    k_CONTROLLER_MISSING_MESSAGE = xTextFindString("TRC_CONTROLLER_MISSING", NULL);
    k_CONTROLLER_INVALID_MESSAGE = xTextFindString("TRC_CONTROLLER_INVALID", NULL);
}

// RESTORED: from Rat GC
static U32 pad_message_valid()
{
    if (gGameMode == eGameMode_Boot || gGameMode == eGameMode_Intro) {
        return FALSE;
    }
    if (zMenuRunning()) {
        return FALSE;
    }
    if (zGameModeGet() == eGameMode_Save || zGameModeGet() == eGameMode_Load) {
        return TRUE;
    }
    return TRUE;
}
// END RESTORED

static void render_message(const char* s)
{
    xColor yellow = { 0xFF, 0xE6, 0x00, 0xFF };
    xtextbox tb = xtextbox::create(xfont::create(0, NSCREENX(19.0f), NSCREENY(22.0f), 0.0f, yellow, screen_bounds), screen_bounds);
    tb.set_text(s);
    tb.bounds = screen_bounds;
    tb.bounds.contract(0.1f);
    tb.bounds.h = tb.yextent(true);
    tb.bounds.y = 0.5f - 0.5f * tb.bounds.h;
    xColor c = { 0x00, 0x00, 0x00, 0xC8 };
    U32 mask = iDrawGetFBMSK();
    iDrawSetFBMSK(0, 0xFFFFFFFF, true);
    render_fill_rect(tb.font.clip, c);
    iDrawSetFBMSK(mask, 0xFFFFFFFF, true);
    tb.render(true);
}

static char composed_message_text[256];
static const char* message_text;

void xTRCClearPadMsg()
{
    message_text = NULL;
    composed_message_text[0] = '\0';
}

void xTRCRender()
{
    if (!globals.cmgr) {
        if (composed_message_text[0] != '\0')
            render_message(composed_message_text);
        else if (message_text)
            render_message(message_text);
    }
}

void xTRCReset()
{
    xTRCClearPadMsg();
}

void xTRCPadGetPressed(TRCState padState, bool& A, bool& B, bool& C, bool& D)
{
    A = B = C = D = false;
    for (S32 k = 0; k < k_XPAD_MAX; k++) {
        if (xTRCPadIsPort(k, padState)) {
            if (mPad[k].pressed & 0x10000) {
                A = true;
            }
            if (mPad[k].pressed & 0x20000) {
                B = true;
            }
            if (mPad[k].pressed & 0x40000) {
                C = true;
            }
            if (mPad[k].pressed & 0x80000) {
                D = true;
            }
        }
    }
}

void xTRCPadSetPressed(S32 port, S32 value, bool reverse)
{
    xASSERT(port != -1);
    xASSERT(value > 0);
    if (!reverse)
        mPad[port].pressed |= value;
    else
        mPad[port].pressed &= ~value;
}

// RESTORED: from Rat GC
static S32 DisplayMessage(S32 port, TRCState state)
{
    S32 display = FALSE;
    switch (state) {
    case TRC_Unknown:
        message_text = NULL;
        break;
    case TRC_PadMissing:
    case TRC_PadInserted:
    case TRC_PadPreInserted:
        if (!pad_message_valid()) {
            return FALSE;
        }
        message_text = k_CONTROLLER_MISSING_MESSAGE;
        U32 len_message_text = strlen(message_text);
        if (len_message_text < 256) {
            const char* msg = message_text;
            char c;
            while ((c = *msg++) != '\0' && c != '<') {}
            if (c == '\0') {
                break;
            }
            U32 sub_len = msg - message_text;
            memcpy(composed_message_text, message_text, sub_len);
            composed_message_text[sub_len - 1] = '1' + port;
            memcpy(composed_message_text + sub_len, &message_text[sub_len + 2], len_message_text - sub_len - 1);
            composed_message_text[len_message_text - 2] = '\0';
        }
        break;
    case TRC_PadInvalidType:
        message_text = k_CONTROLLER_INVALID_MESSAGE;
        break;
    case TRC_DiskNotIdentified:
        message_text = "{i:k_disk_not_identified}";
        break;
    case TRC_DiskTrayOpen:
        message_text = "{i:k_disk_cover_open}";
        break;
    case TRC_DiskNoDisk:
        message_text = "{i:k_disk_no_disk}";
        break;
    case TRC_DiskInvalid:
        message_text = "{i:k_disk_wrong_disk}";
        break;
    case TRC_DiskRetry:
        message_text = "{i:k_disk_retry}";
        break;
    case TRC_DiskFatal:
        message_text = "{i:k_disk_fatal}";
        break;
    default:
        return display;
    }
    display = TRUE;
    return display;
}

void xTRCPadDisplayMessage(S32 pad_id, TRCState)
{
    xASSERT(pad_id >= 0 && pad_id < k_MAX_TRC_PADS);
    if (!globals.NoPadCheck) {
        S32 display_message = DisplayMessage(gTrcPad[pad_id].id, gTrcPad[pad_id].state);
        if (display_message) {
            if (!message_text) {
                message_text = "<error text>";
            }
        } else {
            message_text = NULL;
        }
    }
}
// END RESTORED

void xTRCDisk(TRCState state)
{
    if (state != TRC_DiskNotIdentified) {
        gTrcDisk.state = state;
        gTrcDisk.id = TRC_DiskIdentified;
    } else {
        gTrcDisk.id = TRC_DiskNotIdentified;
    }
}

void render_mem_card_no_space(S32 needed, S32 available, S32 neededFiles, bool enabled)
{
    if (available < 0 && neededFiles != -1 && needed != -1) {
        available = 0;
    }
    bad_card_needed = needed;
    bad_card_available = available;
    const char* error_text = "{i:text_mem_card_no_space}";
    if (neededFiles == 0 && needed > available) {
        error_text = "{i:text_mem_card_no_space_overwrite}";
    }
    else if (neededFiles > 0 || needed > available || neededFiles == -1) {
        error_text = "{i:text_mem_card_no_space_no_save}";
    }
    RenderText(error_text, enabled);
}

void RenderText(const char* text, bool enabled)
{
    xColor c1 = { 0xFF, 0xE6, 0x00, 0xFF };
    static xtextbox tb = xtextbox::create(xfont::create(0, NSCREENX(19.0f), NSCREENY(22.0f), 0.0f, c1, screen_bounds), screen_bounds);
    tb.set_text(enabled ? text : "");
    tb.bounds = screen_bounds;
    tb.bounds.contract(0.1f);
    tb.bounds.h = tb.yextent(true);
    tb.bounds.y = 0.5f - 0.5f * tb.bounds.h;
    tb.render(true);
    xColor c2 = { 0x00, 0x00, 0x00, 0x96 };
    if (!enabled) {
        render_fill_rect(tb.font.clip, c2);
    }
}
