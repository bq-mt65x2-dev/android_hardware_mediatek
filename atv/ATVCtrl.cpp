#define LOG_TAG "ATVCtrl"

#include <utils/Log.h>
#include <cutils/log.h>
#include <binder/IServiceManager.h>
#include <math.h>

#include "ATVCtrl.h"

namespace android
{

Mutex ATVCtrl::mLock;
Mutex ATVCtrl::mATVLock;
Mutex ATVCtrl::mNotifyLock;

sp<IATVCtrlService> ATVCtrl::spATVCtrlService;
sp<ATVCtrl::ATVCtrlClient> ATVCtrl::spATVCtrlClient;
sp<ATVCtrl::ATVCtrlClient> ATVCtrl::spATVCtrlClient_FM;
sp<ATVListener>     ATVCtrl::mListener;
sp<MediaPlayerListener>    ATVCtrl::mFMListener = NULL;

matv_autoscan_progress_chstate_cb ATVCtrl::gATVAutoScan_CB = NULL;
matv_fullscan_progress_cb         ATVCtrl::gATVFullScan_CB = NULL;
matv_scanfinish_cb                ATVCtrl::gATVScanFinish_CB = NULL;
matv_audioformat_cb               ATVCtrl::gATVAudFormat_CB = NULL;
matv_shutdown_cb                   ATVCtrl::gATVShutdown_CB = NULL;

void *ATVCtrl::m_Object = NULL;
Mutex ATVCtrl::mCallbackLock;

struct fm_tune_parm
{
    uint8_t err;
    uint8_t band;
    uint8_t space;
    uint8_t hilo;
    uint16_t freq; // IN/OUT parameter
};

struct fm_seek_parm
{
    uint8_t err;
    uint8_t band;
    uint8_t space;
    uint8_t hilo;
    uint8_t seekdir;
    uint8_t seekth;
    uint16_t freq; // IN/OUT parameter
};

//for hw scan
struct fm_scan_parm
{
    uint8_t  err;
    uint8_t  band;
    uint8_t  space;
    uint8_t  hilo;
    uint16_t freq; // OUT parameter
    uint16_t ScanTBL[16]; //need no less than the chip
    uint16_t ScanTBLSize; //IN/OUT parameter
};


// establish binder interface to ATVCtrlService service
const sp<IATVCtrlService>& ATVCtrl::get_ATVCtrlService()
{
    SLOGD("get_ATVCtrlService()");
    Mutex::Autolock _l(mLock);

    if (spATVCtrlService.get() == 0 || spATVCtrlClient == NULL)
    {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;

        do
        {
            binder = sm->getService(String16("media.ATVCtrlService"));

            if (binder != 0)
            {
                break;
            }

            SLOGW("ATVCtrlService not published, waiting...");
            usleep(500000); // 0.5 s
        }
        while (true);

        if (spATVCtrlClient == NULL)
        {
            spATVCtrlClient = new ATVCtrlClient();
        }
        else
        {
            /*
                     if (gATVErrorCallback){
                            gATVErrorCallback(NO_ERROR);
                     }
            */
            SLOGD(" spATVCtrlClient exist ");
        }

        binder->linkToDeath(spATVCtrlClient);
        spATVCtrlService = interface_cast<IATVCtrlService>(binder);
        spATVCtrlService->registerClient(spATVCtrlClient);
        SLOGD(" ATVCtrlService is published, or new spATVCtrlClient ");
    }

    if (spATVCtrlService == 0)
    {
        SLOGE("no ATVCtrlService!?");
    }

    return spATVCtrlService;
}

const sp<IATVCtrlService>& ATVCtrl::get_ATVCtrlService_FM()
{
    SLOGD("get_ATVCtrlService_FM()");
    Mutex::Autolock _l(mLock);

    if (spATVCtrlService.get() == 0 || spATVCtrlClient_FM == NULL)
    {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;

        do
        {
            binder = sm->getService(String16("media.ATVCtrlService"));

            if (binder != 0)
            {
                break;
            }

            SLOGW("ATVCtrlService not published, waiting...");
            usleep(500000); // 0.5 s
        }
        while (true);

        if (spATVCtrlClient_FM == NULL)
        {
            spATVCtrlClient_FM = new ATVCtrlClient();
        }
        else
        {
            /*
                     if (gATVErrorCallback){
                            gATVErrorCallback(NO_ERROR);
                     }
            */
            SLOGD(" spATVCtrlClient_FM exist ");
        }

        binder->linkToDeath(spATVCtrlClient_FM);
        spATVCtrlService = interface_cast<IATVCtrlService>(binder);
        spATVCtrlService->registerClient_FM(spATVCtrlClient_FM);
        SLOGD(" ATVCtrlService is published, or new spATVCtrlClient_FM ");
    }

    if (spATVCtrlService == 0)
    {
        SLOGE("no ATVCtrlService!?");
    }

    return spATVCtrlService;
}

const sp<ATVListener>& ATVCtrl::get_ATVListener()
{
    return mListener;
}
ATVCtrl::ATVCtrl()
{
    //m_Object = NULL;
}

ATVCtrl::~ATVCtrl()
{

}

int ATVCtrl::ATVC_matv_init()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_init");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_init();
    return ret;
}

int ATVCtrl::ATVC_matv_ps_init(int on)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_init on = %d", on);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_ps_init(on);
    return ret;
}

int ATVCtrl::ATVC_matv_suspend(int on)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_suspend on=%d", on);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_suspend(on);
    return ret;
}
int ATVCtrl::ATVC_matv_shutdown()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_shutdown");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_shutdown();

    spATVCtrlClient = NULL;
    return ret;
}
void ATVCtrl::ATVC_matv_chscan(int mode)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_chscan mode=%d", mode);

    if (atvcs == 0)
    {
        return;
    }

    atvcs->ATVCS_matv_chscan(mode);
}
void ATVCtrl::ATVC_matv_chscan_stop()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_chscan_stop");

    if (atvcs == 0)
    {
        return;
    }

    atvcs->ATVCS_matv_chscan_stop();
}
int ATVCtrl::ATVC_matv_get_chtable(int ch, matv_ch_entry *entry)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_get_chtable ch=%d, entry=%p", ch, entry);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret;
    void *ptr = (void *)entry;
    int len = sizeof(matv_ch_entry);
    SLOGD("ATVC_matv_get_chtable len=%d", len);
    ret = atvcs->ATVCS_matv_get_chtable(ch, ptr, len);
    return ret;
}
int ATVCtrl::ATVC_matv_set_chtable(int ch, matv_ch_entry *entry)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_set_chtable ch=%d, entry=%p", ch, entry);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret;
    void *ptr = (void *)entry;
    int len = sizeof(matv_ch_entry);
    SLOGD("ATVC_matv_set_chtable len=%d", len);
    ret = atvcs->ATVCS_matv_set_chtable(ch, ptr, len);
    return ret;
}
int ATVCtrl::ATVC_matv_clear_chtable()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_clear_chtable");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_clear_chtable();
    return ret;
}
void ATVCtrl::ATVC_matv_change_channel(int ch)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_change_channel ch=%d", ch);

    if (atvcs == 0)
    {
        return;
    }

    atvcs->ATVCS_matv_change_channel(ch);
}
void ATVCtrl::ATVC_matv_set_country(int country)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_set_country country=%d", country);

    if (atvcs == 0)
    {
        return;
    }

    atvcs->ATVCS_matv_set_country(country);
}
void ATVCtrl::ATVC_matv_set_tparam(int mode)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_set_tparam mode=%d", mode);

    if (atvcs == 0)
    {
        return;
    }

    atvcs->ATVCS_matv_set_tparam(mode);
}
void ATVCtrl::ATVC_matv_audio_play()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_audio_play");

    if (atvcs == 0)
    {
        return;
    }

    atvcs->ATVCS_matv_audio_play();
}
void ATVCtrl::ATVC_matv_audio_stop()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_audio_stop");

    if (atvcs == 0)
    {
        return;
    }

    atvcs->ATVCS_matv_audio_stop();
}
int ATVCtrl::ATVC_matv_audio_get_format()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_audio_get_format");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_audio_get_format();
    return ret;
}
void ATVCtrl::ATVC_matv_audio_set_format(int val)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_audio_set_format val=%d", val);

    if (atvcs == 0)
    {
        return;
    }

    atvcs->ATVCS_matv_audio_set_format(val);
}
int ATVCtrl::ATVC_matv_audio_get_sound_system()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_audio_get_sound_system");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_audio_get_sound_system();
    return ret;
}
int ATVCtrl::ATVC_matv_adjust(int item, int val)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_adjust item=%d, val=%d", item, val);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_adjust(item, val);
    return ret;
}
int ATVCtrl::ATVC_matv_get_chipdep(int item)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_get_chipdep item=%d", item);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_get_chipdep(item);
    return ret;
}
int ATVCtrl::ATVC_matv_set_chipdep(int item, int val)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();
    SLOGD("ATVC_matv_set_chipdep item=%d val=%d", item, val);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_matv_set_chipdep(item, val);
    return ret;
}

int ATVCtrl::ATVC_fm_setListener(sp<MediaPlayerListener> listener)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    SLOGD("ATVC_fm_setListener");

    if (atvcs == 0)
    {
        SLOGD("ATVC_fm_setListener error");
        return PERMISSION_DENIED;
    }
    else
    {
        SLOGD("ATVC_fm_setListener success");
        mFMListener = listener;
        return 1;
    }

}

int ATVCtrl::ATVC_fm_powerup(void /*struct fm_tune_parm*/ *t_parm)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    struct fm_tune_parm *parm = (struct fm_tune_parm *)t_parm;
    SLOGD("ATVC_fm_powerup parm=%p", parm);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret;
    void *ptr = (void *)parm;
    int len = sizeof(struct fm_tune_parm);
    SLOGD("ATVC_fm_powerup len=%d", len);
    ret = atvcs->ATVCS_fm_powerup(ptr, len);
    return ret;
}
int ATVCtrl::ATVC_fm_powerdown()
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    SLOGD("ATVC_fm_powerdown");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_fm_powerdown();
    return ret;
}
int ATVCtrl::ATVC_fm_getrssi(int *val)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    SLOGD("ATVC_fm_getrssi");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    *val = atvcs->ATVCS_fm_getrssi();
    return NO_ERROR;
}
int ATVCtrl::ATVC_fm_tune(void /*struct fm_tune_parm*/ *t_parm)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    struct fm_tune_parm *parm = (struct fm_tune_parm *)t_parm;
    SLOGD("ATVC_fm_tune parm=%p", parm);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret;
    void *ptr = (void *)parm;
    int len = sizeof(struct fm_tune_parm);
    SLOGD("ATVC_fm_tune len=%d", len);
    ret = atvcs->ATVCS_fm_tune(ptr, len);
    return ret;
}
int ATVCtrl::ATVC_fm_seek(void/*struct fm_seek_parm*/ *s_parm)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    struct fm_seek_parm *parm = (struct fm_seek_parm *)s_parm;
    SLOGD("ATVC_fm_seek parm=%p", parm);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret;
    void *ptr = (void *)parm;
    int len = sizeof(struct fm_seek_parm);
    SLOGD("ATVC_fm_seek len=%d", len);
    ret = atvcs->ATVCS_fm_seek(ptr, len);
    return ret;
}
int ATVCtrl::ATVC_fm_scan(void/*struct fm_scan_parm*/ *s_parm)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    struct fm_scan_parm *parm = (struct fm_scan_parm *)s_parm;
    SLOGD("ATVC_fm_seek parm=%p", parm);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret;
    void *ptr = (void *)parm;
    int len = sizeof(struct fm_scan_parm);
    SLOGD("ATVC_fm_seek len=%d", len);
    ret = atvcs->ATVCS_fm_scan(ptr, len);
    return ret;
}
int ATVCtrl::ATVC_fm_mute(int val)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    SLOGD("ATVC_fm_mute val=%d", val);

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    int ret = atvcs->ATVCS_fm_mute(val);
    return ret;
}
int ATVCtrl::ATVC_fm_getchipid(unsigned short int *val)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    SLOGD("ATVC_fm_getchipid");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    *val = (unsigned short int)atvcs->ATVCS_fm_getchipid();
    return NO_ERROR;
}
int ATVCtrl::ATVC_fm_isRDSsupport(int *val)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    SLOGD("ATVC_fm_isRDSsupport");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    *val = false;
    return NO_ERROR;
}
int ATVCtrl::ATVC_fm_isFMPowerUp(int *val)
{
    Mutex::Autolock _l(mATVLock);
    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService_FM();
    SLOGD("ATVC_fm_isFMPowerUp");

    if (atvcs == 0)
    {
        return PERMISSION_DENIED;
    }

    *val = atvcs->ATVCS_fm_isFMPowerUp();
    return NO_ERROR;
}
// -------------------------------------------------------------------------------------------

// DeathRecipient
void ATVCtrl::ATVCtrlClient::binderDied(const wp<IBinder>& /*who*/)
{
    SLOGD("binderDied!");
    Mutex::Autolock _l(ATVCtrl::mLock);
    ATVCtrl::spATVCtrlService.clear();
    SLOGD("ATVCtrlService server died!");
}

// IATVCtrlClient ==========================================================================
void ATVCtrl::ATVCtrlClient::AutoScan_CB(int precent, int ch, int chnum, void *ptr, int len)
{
    Mutex::Autolock _l(mCallbackLock);
    SLOGD("ATVCtrl::ATVCtrlClient::AutoScan_CB! m_Object=%p, precent=%d,ch=%d,chnum=%d,ptr=%p,len=%d", m_Object, precent, ch, chnum, ptr, len);
    /*if ( gATVAutoScan_CB != NULL)
    gATVAutoScan_CB((void *)m_Object,precent,ch,chnum, ptr, len);*/
    sp<ATVListener> pListener = ATVCtrl::get_ATVListener();

    if (pListener.get() != NULL)
    {
        pListener->autoscan_progress_chstate_cb(precent, ch, chnum, ptr, len);
    }
}

void ATVCtrl::ATVCtrlClient::FullScan_CB(int precent, int freq, int freq_start, int freq_end)
{
    Mutex::Autolock _l(mCallbackLock);
    SLOGD("ATVCtrl::ATVCtrlClient::FullScan_CB! m_Object=%p, precent=%d,freq=%d,freq_start=%d,freq_end=%d", m_Object, precent, freq, freq_start, freq_end);
    /*if ( gATVFullScan_CB != NULL)
    gATVFullScan_CB((void *)m_Object,precent,freq,freq_start,freq_end);*/
    sp<ATVListener> pListener = ATVCtrl::get_ATVListener();

    if (pListener.get() != NULL)
    {
        pListener->fullscan_progress_cb(precent, freq, freq_start, freq_end);
    }
}

void ATVCtrl::ATVCtrlClient::ScanFinish_CB(int chnum)
{
    Mutex::Autolock _l(mCallbackLock);
    SLOGD("ATVCtrl::ATVCtrlClient::ScanFinish_CB! m_Object=%p, chnum=%d", m_Object, chnum);
    /* if ( gATVScanFinish_CB != NULL)
     gATVScanFinish_CB((void *)m_Object,chnum);*/
    sp<ATVListener> pListener = ATVCtrl::get_ATVListener();

    if (pListener.get() != NULL)
    {
        pListener->scanfinish_cb(chnum);
    }
}

void ATVCtrl::ATVCtrlClient::AudioFmt_CB(int format)
{
    Mutex::Autolock _l(mCallbackLock);
    SLOGD("ATVCtrl::ATVCtrlClient::AudioFmt_CB! m_Object=%p, format=%d", m_Object, format);
    /*if ( gATVAudFormat_CB != NULL)
    gATVAudFormat_CB((void *)m_Object,format);*/
    sp<ATVListener> pListener = ATVCtrl::get_ATVListener();

    if (pListener.get() != NULL)
    {
        pListener->audioformat_cb(format);
    }
}

void ATVCtrl::ATVCtrlClient::Shutdown_CB(int source)
{
    Mutex::Autolock _l(mCallbackLock);
    SLOGD("ATVCtrl::ATVCtrlClient::Shutdown_CB! m_Object=%p, format=%d", m_Object, source);
    /*if ( gATVShutdown_CB != NULL)
    gATVShutdown_CB((void *)m_Object,source);*/
    sp<ATVListener> pListener = ATVCtrl::get_ATVListener();

    if (pListener.get() != NULL)
    {
        pListener->shutdown_cb(source);
    }
}

void ATVCtrl::ATVC_matv_setListener(const sp<ATVListener>& listener)
{
    Mutex::Autolock _l(mATVLock);
    mListener = listener;

    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();

    if (atvcs == NULL)
    {
        SLOGD("ATVC_matv_setListener atvcs = 0 ");
        return;
    }

    return atvcs->ATVCS_matv_register_callback();
}

void ATVCtrl::ATVCtrlClient::notify(int msg, int ext1, int ext2)
{
    ATVCtrl::notify(msg, ext1, ext2);
}

void ATVCtrl::notify(int msg, int ext1, int ext2)
{
    sp<MediaPlayerListener> listener = mFMListener;
    SLOGD("ATVCtrl::notify");

    // this prevents re-entrant calls into client code
    if (listener.get() != 0)
    {
        Mutex::Autolock _l(mNotifyLock);
        SLOGD("NOTIFY APP +++,MSG %d", msg);
        listener->notify(msg, ext1, ext2, NULL);
        SLOGD("NOTIFY APP ---,MSG %d", msg);
    }
}

void ATVCtrl::ATVC_matv_register_callback(void *cb_param,
        matv_autoscan_progress_chstate_cb auto_cb,
        matv_fullscan_progress_cb full_cb,
        matv_scanfinish_cb finish_cb,
        matv_audioformat_cb audfmt_cb,
        matv_shutdown_cb shutdown_cb)
{
    Mutex::Autolock _l(mATVLock);
    SLOGD("ATVC_matv_register_callback cb_param=%p", cb_param);
    SLOGD("ATVC_matv_register_callback auto_cb=%p, full_cb=%p, finish_cb=%p, audfmt_cb=%p, shutdown_cb=%p", auto_cb, full_cb, finish_cb, audfmt_cb, shutdown_cb);
    gATVAutoScan_CB = auto_cb;
    gATVFullScan_CB = full_cb;
    gATVScanFinish_CB = finish_cb;
    gATVAudFormat_CB  = audfmt_cb;
    gATVShutdown_CB  = shutdown_cb;
    m_Object = cb_param;

    const sp<IATVCtrlService>& atvcs = ATVCtrl::get_ATVCtrlService();

    if (atvcs == NULL)
    {
        SLOGD("ATVC_matv_register_callback atvcs = 0 ");
        return;
    }

    return atvcs->ATVCS_matv_register_callback();
}

};
