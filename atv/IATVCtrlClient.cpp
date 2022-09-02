#define LOG_TAG "IATVCtrlClient"

#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <cutils/log.h>

#include "IATVCtrlClient.h"

namespace android
{

enum
{
    AUTO_SCAN_CB = IBinder::FIRST_CALL_TRANSACTION,
    FULL_SCAN_CB,
    SCAN_FINISH_CB,
    AUDIO_FORMAT_CB,
    SHUTDOWN_CB,
    NOTIFY
};

class BpATVCtrlClient : public BpInterface<IATVCtrlClient>
{
public:
    BpATVCtrlClient(const sp<IBinder>& impl)
        : BpInterface<IATVCtrlClient>(impl)
    {
    }

    virtual void AutoScan_CB(int precent, int ch, int chnum, void *ptr, int len)
    {
        Parcel data, reply;
        SLOGD("AutoScan_CB precent=%d, ch=%d, chnum=%d, ptr=%p, len=%d", precent, ch, chnum, ptr, len);
        data.writeInterfaceToken(IATVCtrlClient::getInterfaceDescriptor());
        data.writeInt32(precent);
        data.writeInt32(ch);
        data.writeInt32(chnum);

        data.writeInt32(len);
        data.write(ptr, len);
        remote()->transact(AUTO_SCAN_CB, data, &reply);
        reply.read(ptr, len);
    }
    virtual void FullScan_CB(int precent, int freq, int freq_start, int freq_end)
    {
        Parcel data, reply;
        SLOGD("FullScan_CB precent=%d, freq=%d, freq_start=%d, freq_end=%d", precent, freq, freq_start, freq_end);
        data.writeInterfaceToken(IATVCtrlClient::getInterfaceDescriptor());
        data.writeInt32(precent);
        data.writeInt32(freq);
        data.writeInt32(freq_start);
        data.writeInt32(freq_end);
        remote()->transact(FULL_SCAN_CB, data, &reply);
    }
    virtual void ScanFinish_CB(int chnum)
    {
        Parcel data, reply;
        SLOGD("ScanFinish_CB chnum=%d", chnum);
        data.writeInterfaceToken(IATVCtrlClient::getInterfaceDescriptor());
        data.writeInt32(chnum);
        remote()->transact(SCAN_FINISH_CB, data, &reply);
    }
    virtual void AudioFmt_CB(int format)
    {
        Parcel data, reply;
        SLOGD("AudioFmt_CB format=%d", format);
        data.writeInterfaceToken(IATVCtrlClient::getInterfaceDescriptor());
        data.writeInt32(format);
        remote()->transact(AUDIO_FORMAT_CB, data, &reply);
    }
    virtual void Shutdown_CB(int source)
    {
        Parcel data, reply;
        SLOGD("Shutdown_CB format=%d", source);
        data.writeInterfaceToken(IATVCtrlClient::getInterfaceDescriptor());
        data.writeInt32(source);
        remote()->transact(SHUTDOWN_CB, data, &reply);
    }
    virtual void notify(int msg, int ext1, int ext2)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IATVCtrlClient::getInterfaceDescriptor());
        data.writeInt32(msg);
        data.writeInt32(ext1);
        data.writeInt32(ext2);
        remote()->transact(NOTIFY, data, &reply);
    }

};

IMPLEMENT_META_INTERFACE(ATVCtrlClient, "android.media.IATVCtrlClient");

status_t BnATVCtrlClient::onTransact(
    uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    switch (code)
    {
        case AUTO_SCAN_CB:
        {
            CHECK_INTERFACE(IATVCtrlClient, data, reply);
            int precent = data.readInt32();
            int ch = data.readInt32();
            int chnum = data.readInt32();

            int len = data.readInt32();
            void *ptr = malloc(len);
            data.read(ptr, len);
            SLOGD("onTransact:AUTO_SCAN_CB precent=%d, ch=%d, chnum=%d, ptr=%p, len=%d", precent, ch, chnum, ptr, len);
            AutoScan_CB(precent, ch, chnum, ptr, len);
            reply->write(ptr, len);
            free(ptr);

            return NO_ERROR;
        }
        break;

        case FULL_SCAN_CB:
        {
            CHECK_INTERFACE(IATVCtrlClient, data, reply);
            int precent = data.readInt32();
            int freq = data.readInt32();
            int freq_start = data.readInt32();
            int freq_end = data.readInt32();
            SLOGD("onTransact:FULL_SCAN_CB precent=%d, freq=%d, freq_start=%d, freq_end=%d", precent, freq, freq_start, freq_end);
            FullScan_CB(precent, freq, freq_start, freq_end);
            return NO_ERROR;
        }
        break;

        case SCAN_FINISH_CB:
        {
            CHECK_INTERFACE(IATVCtrlClient, data, reply);
            int chnum = data.readInt32();
            SLOGD("onTransact:SCAN_FINISH_CB chnum=%d", chnum);
            ScanFinish_CB(chnum);
            return NO_ERROR;
        }
        break;

        case AUDIO_FORMAT_CB:
        {
            CHECK_INTERFACE(IATVCtrlClient, data, reply);
            int format = data.readInt32();
            SLOGD("onTransact:AUDIO_FORMAT_CB format=%d", format);
            AudioFmt_CB(format);
            return NO_ERROR;
        }
        break;

        case SHUTDOWN_CB:
        {
            CHECK_INTERFACE(IATVCtrlClient, data, reply);
            int source = data.readInt32();
            SLOGD("onTransact:SHUTDOWN_CB source=%d", source);
            Shutdown_CB(source);
            return NO_ERROR;
        }
        break;

        case NOTIFY:
        {
            CHECK_INTERFACE(IATVCtrlClient, data, reply);
            int msg = data.readInt32();
            int ext1 = data.readInt32();
            int ext2 = data.readInt32();
            notify(msg, ext1, ext2);
            return NO_ERROR;
        }
        break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}
};
