#ifndef ANDROID_IATVCTRLCLIENT_H
#define ANDROID_IATVCTRLCLIENT_H

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android
{

class IATVCtrlClient : public IInterface
{
public:
    DECLARE_META_INTERFACE(ATVCtrlClient);

    virtual void AutoScan_CB(int precent, int ch, int chnum, void *ptr, int len) = 0;
    virtual void FullScan_CB(int precent, int freq, int freq_start, int freq_end) = 0;
    virtual void ScanFinish_CB(int chnum)   = 0;
    virtual void AudioFmt_CB(int format) = 0;
    virtual void Shutdown_CB(int format) = 0;
    virtual void notify(int msg, int ext1, int ext2) = 0;
};

class BnATVCtrlClient : public BnInterface<IATVCtrlClient>
{
public:
    virtual status_t    onTransact(uint32_t code,
                                   const Parcel &data,
                                   Parcel *reply,
                                   uint32_t flags = 0);
};

};

#endif // ANDROID_IATVCTRLCLIENT_H
