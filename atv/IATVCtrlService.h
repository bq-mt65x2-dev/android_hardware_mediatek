#ifndef ANDROID_IATVCTRLSERVICE_H
#define ANDROID_IATVCTRLSERVICE_H

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <binder/IInterface.h>
#include "IATVCtrlClient.h"

namespace android
{

class IATVCtrlService : public IInterface
{
public:
    DECLARE_META_INTERFACE(ATVCtrlService);

    // IATVCtrlService interface
    // On/Off Control
    virtual int ATVCS_matv_init(void) = 0;
    virtual int ATVCS_matv_ps_init(int on) = 0;
    virtual int ATVCS_matv_set_parameterb(int on) = 0;
    virtual int ATVCS_matv_suspend(int on) = 0;
    virtual int ATVCS_matv_shutdown(void) = 0;

    // Channel Control
    virtual void ATVCS_matv_chscan(int mode) = 0;
    virtual void ATVCS_matv_chscan_stop(void) = 0;

    virtual int ATVCS_matv_get_chtable(int ch, void *entry, int len)  = 0;
    virtual int ATVCS_matv_set_chtable(int ch, void *entry, int len)  = 0;
    virtual int ATVCS_matv_clear_chtable(void) = 0;

    virtual void ATVCS_matv_change_channel(int ch) = 0;
    virtual void ATVCS_matv_set_country(int country) = 0;
    virtual void ATVCS_matv_set_tparam(int mode) = 0;

    // Audio Control
    virtual void ATVCS_matv_audio_play(void) = 0;
    virtual void ATVCS_matv_audio_stop(void) = 0;
    virtual int ATVCS_matv_audio_get_format(void) = 0;
    virtual void ATVCS_matv_audio_set_format(int val) = 0;
    virtual int ATVCS_matv_audio_get_sound_system(void) = 0;

    // Adjustment
    virtual int ATVCS_matv_adjust(int item, int val) = 0;

    // Meta
    virtual int ATVCS_matv_get_chipdep(int item) = 0;
    virtual int ATVCS_matv_set_chipdep(int item, int val) = 0;

    //FM
    virtual int ATVCS_fm_powerup(void *parm, int len) = 0;
    virtual int ATVCS_fm_powerdown(void) = 0;
    virtual int ATVCS_fm_getrssi(void) = 0;
    virtual int ATVCS_fm_tune(void *parm, int len) = 0;
    virtual int ATVCS_fm_seek(void *parm, int len) = 0;
    virtual int ATVCS_fm_scan(void *parm, int len) = 0;
    virtual int ATVCS_fm_mute(int val) = 0;
    virtual int ATVCS_fm_getchipid(void) = 0;
    virtual int ATVCS_fm_isFMPowerUp(void) = 0;

    // Callback
    virtual void ATVCS_matv_register_callback() = 0;

    // register a current process for audio output change notifications
    virtual void registerClient(const sp<IATVCtrlClient>& client) = 0;
    virtual void registerClient_FM(const sp<IATVCtrlClient>& client) = 0;

    // CLI
    virtual void CLI(char input) = 0;
};


class BnATVCtrlService : public BnInterface<IATVCtrlService>
{
public:
    virtual status_t    onTransact(uint32_t code,
                                   const Parcel &data,
                                   Parcel *reply,
                                   uint32_t flags = 0);
};

};

#endif // ANDROID_IATVCTRLSERVICE_H
