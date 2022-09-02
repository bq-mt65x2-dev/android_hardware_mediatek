#ifndef ANDROID_ATVCTRL_H_
#define ANDROID_ATVCTRL_H_

#include <utils/RefBase.h>
#include <utils/threads.h>
#include <media/mediaplayer.h>

#include "IATVCtrlService.h"

namespace android
{

typedef int kal_bool;
typedef signed char  kal_int8;
typedef unsigned char  kal_uint8;
typedef signed short  kal_int16;
typedef unsigned short  kal_uint16;
typedef signed int    kal_int32;
typedef unsigned int  kal_uint32;

//callback function for return scan state
typedef  void (*matv_autoscan_progress_chstate_cb)(void *cb_param, kal_uint8 precent, kal_uint8 ch, kal_uint8 chnum, void *ptr, int len);

// callback function
typedef  void (*matv_autoscan_progress_cb)(void *cb_param, kal_uint8 precent, kal_uint8 ch, kal_uint8 chnum);
typedef  void (*matv_fullscan_progress_cb)(void *cb_param, kal_uint8 precent, kal_uint32 freq, kal_uint32 freq_start, kal_uint32 freq_end);
typedef  void (*matv_scanfinish_cb)(void *cb_param, kal_uint8 chnum);
typedef  void (*matv_audioformat_cb)(void *cb_param, kal_uint32 format);
typedef  void (*matv_shutdown_cb)(void *cb_param, kal_uint32 source);

typedef struct
{
    kal_uint32  freq; //khz
    kal_uint8   sndsys; /* reference sv_const.h, TV_AUD_SYS_T ...*/
    kal_uint8   colsys; /* reference sv_const.h, SV_CS_PAL_N, SV_CS_PAL,SV_CS_NTSC358...*/
    kal_uint8   flag;
} matv_ch_entry;

// ref-counted object for callbacks
class ATVListener:  virtual public RefBase
{
public:
    virtual  void autoscan_progress_chstate_cb(kal_uint8 precent, kal_uint8 ch, kal_uint8 chnum, void *ptr, int len) = 0;
    virtual  void fullscan_progress_cb(kal_uint8 precent, kal_uint32 freq, kal_uint32 freq_start, kal_uint32 freq_end) = 0;
    virtual  void scanfinish_cb(kal_uint8 chnum) = 0;
    virtual  void audioformat_cb(kal_uint32 format) = 0;
    virtual  void shutdown_cb(kal_uint32 source) = 0;
};

class ATVCtrl
{
public:

    ATVCtrl();
    ~ATVCtrl();

    // ATVCtrl interface ============================================================
    // On/Off Control
    static int ATVC_matv_init(void);
    static int ATVC_matv_ps_init(int on);
    static int ATVC_matv_suspend(int on);
    static int ATVC_matv_shutdown(void);
    // Channel Control
    static void ATVC_matv_chscan(int mode);
    static void ATVC_matv_chscan_stop(void);
    static int ATVC_matv_get_chtable(int ch, matv_ch_entry *entry);
    static int ATVC_matv_set_chtable(int ch, matv_ch_entry *entry);

    static int ATVC_matv_clear_chtable(void);
    static void ATVC_matv_change_channel(int ch);
    static void ATVC_matv_set_country(int country);
    static void ATVC_matv_set_tparam(int mode);

    // Audio Control
    static void ATVC_matv_audio_play(void);
    static void ATVC_matv_audio_stop(void);
    static int ATVC_matv_audio_get_format(void);
    static void ATVC_matv_audio_set_format(int val);
    static int ATVC_matv_audio_get_sound_system(void);

    // Adjustment
    static int ATVC_matv_adjust(int item, int val);

    // Meta
    static int ATVC_matv_get_chipdep(int item);
    static int ATVC_matv_set_chipdep(int item, int val);
    static void ATVC_matv_setListener(const sp<ATVListener>& listener);

    //FM
    static void notify(int msg, int ext1, int ext2);
    static int ATVC_fm_setListener(sp<MediaPlayerListener> listener);
    static int ATVC_fm_powerup(void/*struct fm_tune_parm*/ *t_parm);
    static int ATVC_fm_powerdown(void);
    static int ATVC_fm_getrssi(int *val);
    static int ATVC_fm_tune(void/*struct fm_tune_parm*/ *t_parm);
    static int ATVC_fm_seek(void/*struct fm_seek_parm*/ *s_parm);
    static int ATVC_fm_scan(void/*struct fm_scan_parm*/ *s_parm);
    static int ATVC_fm_mute(int val);
    static int ATVC_fm_getchipid(unsigned short int *val);
    static int ATVC_fm_isRDSsupport(int *val);
    static int ATVC_fm_isFMPowerUp(int *val);

    // Callback
    static void ATVC_matv_register_callback(void *cb_param,
                                            matv_autoscan_progress_chstate_cb auto_cb,
                                            matv_fullscan_progress_cb full_cb,
                                            matv_scanfinish_cb finish_cb,
                                            matv_audioformat_cb audfmt_cb,
                                            matv_shutdown_cb shutdown_cb);

    // helper function to obtain ATVCtrl service handle
    static const sp<IATVCtrlService>& get_ATVCtrlService();
    static const sp<IATVCtrlService>& get_ATVCtrlService_FM();
    static const sp<ATVListener>& get_ATVListener();

private:

    class ATVCtrlClient: public IBinder::DeathRecipient, public BnATVCtrlClient
    {
    public:
        ATVCtrlClient()
        {
        }
        // DeathRecipient
        virtual void binderDied(const wp<IBinder>& who);

        // IATVCtrlClient
        virtual void AutoScan_CB(int precent, int ch, int chnum, void *ptr, int len);
        virtual void FullScan_CB(int precent, int freq, int freq_start, int freq_end);
        virtual void ScanFinish_CB(int chnum);
        virtual void AudioFmt_CB(int format);
        virtual void Shutdown_CB(int source);
        virtual void notify(int msg, int ext1, int ext2);
    };

    static sp<ATVCtrlClient> spATVCtrlClient;
    static sp<ATVCtrlClient> spATVCtrlClient_FM;
    friend class ATVCtrlClient;
    static Mutex mLock;
    static sp<IATVCtrlService> spATVCtrlService;
    static sp<ATVListener>     mListener;

    static matv_autoscan_progress_chstate_cb gATVAutoScan_CB;
    static matv_fullscan_progress_cb gATVFullScan_CB;
    static matv_scanfinish_cb        gATVScanFinish_CB;
    static matv_audioformat_cb       gATVAudFormat_CB;
    static matv_shutdown_cb       gATVShutdown_CB;
    static void *m_Object;

    static Mutex mATVLock;
    static Mutex mCallbackLock;
    static Mutex                       mNotifyLock;
    static sp<MediaPlayerListener>     mFMListener;

};

};

#endif  /*ANDROID_ATVCTRL_H_*/
