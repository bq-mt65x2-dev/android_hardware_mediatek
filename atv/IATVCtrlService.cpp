#define LOG_TAG "IATVCtrlService"

#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <cutils/log.h>

#include "IATVCtrlService.h"

namespace android
{

enum
{
    ATV_INIT = IBinder::FIRST_CALL_TRANSACTION,
    ATV_PS_INIT,
    ATV_SET_PARAMETERB,
    ATV_SUSPEND,
    ATV_SHUTDOWN,
    ATV_CH_SCAN,
    ATV_CH_SCAN_STOP,
    ATV_GET_CH_TABLE,
    ATV_SET_CH_TABLE,
    ATV_CLEAR_CH_TABLE,
    ATV_CHANGE_CH,
    ATV_SET_COUNTRY,
    ATV_SET_TPARAM,
    ATV_AUDIO_PLAY,
    ATV_AUDIO_STOP,
    ATV_AUDIO_GET_FORMAT,
    ATV_AUDIO_SET_FORMAT,
    ATV_GET_SOUND_SYSTEM,
    ATV_ADJUST,
    ATV_GET_CHIPDEP,  // get signal SNR
    ATV_SET_CHIPDEP,  // set signal SNR
    ATV_REG_CALLBACK,
    REGISTER_CLIENT,
    REGISTER_CLIENT_FM,
    ATV_CLI,
    FM_POWERUP,
    FM_POWERDOWN,
    FM_GETRSSI,
    FM_TUNE,
    FM_SEEK,
    FM_SCAN,
    FM_MUTE,
    FM_GETCHIPID,
    FM_ISFMPOWERUP
};

class BpATVCtrlService : public BpInterface<IATVCtrlService>
{
public:
    BpATVCtrlService(const sp<IBinder>& impl)
        : BpInterface<IATVCtrlService>(impl)
    {
    }
    virtual int ATVCS_matv_init()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_init");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_INIT, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_matv_ps_init(int on)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_ps_init:%d", on);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(on);
        remote()->transact(ATV_PS_INIT, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_matv_set_parameterb(int in)
    {
        Parcel data, reply;
        SLOGD("ATVCS_set_parameterb:%d", in);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(in);
        remote()->transact(ATV_SET_PARAMETERB, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_matv_suspend(int on)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_suspend on:%d", on);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(on);
        remote()->transact(ATV_SUSPEND, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_matv_shutdown()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_shutdown");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_SHUTDOWN, data, &reply);
        return reply.readInt32();
    }
    virtual void ATVCS_matv_chscan(int mode)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_chscan mode:%d", mode);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(mode);
        remote()->transact(ATV_CH_SCAN, data, &reply);
    }
    virtual void ATVCS_matv_chscan_stop()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_chscan_stop");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_CH_SCAN_STOP, data, &reply);
    }
    virtual int ATVCS_matv_get_chtable(int ch, void *entry, int len)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_get_chtable ch=%d, entry=%p, len=%d", ch, entry, len);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(ch);
        data.writeInt32(len);
        data.write(entry, len);
        remote()->transact(ATV_GET_CH_TABLE, data, &reply);
        int ret = reply.readInt32();
        reply.read(entry, len);
        return ret;
    }
    virtual int ATVCS_matv_set_chtable(int ch, void *entry, int len)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_set_chtable ch=%d, entry=%p, len=%d", ch, entry, len);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(ch);
        data.writeInt32(len);
        data.write(entry, len);
        remote()->transact(ATV_SET_CH_TABLE, data, &reply);
        int ret = reply.readInt32();
        reply.read(entry, len);
        return ret;
    }
    virtual int ATVCS_matv_clear_chtable()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_clear_chtable");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_CLEAR_CH_TABLE, data, &reply);
        return reply.readInt32();
    }
    virtual void ATVCS_matv_change_channel(int ch)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_change_channel ch=%d", ch);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(ch);
        remote()->transact(ATV_CHANGE_CH, data, &reply);
    }
    virtual void ATVCS_matv_set_country(int country)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_set_country country=%d", country);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(country);
        remote()->transact(ATV_SET_COUNTRY, data, &reply);
    }
    virtual void ATVCS_matv_set_tparam(int mode)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_set_tparam mode=%d", mode);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(mode);
        remote()->transact(ATV_SET_TPARAM, data, &reply);
    }
    virtual void ATVCS_matv_audio_play()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_audio_play");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_AUDIO_PLAY, data, &reply);
    }
    virtual void ATVCS_matv_audio_stop()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_audio_stop");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_AUDIO_STOP, data, &reply);
    }
    virtual int ATVCS_matv_audio_get_format()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_audio_get_format");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_AUDIO_GET_FORMAT, data, &reply);
        return reply.readInt32();
    }
    virtual void ATVCS_matv_audio_set_format(int val)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_audio_set_format val=%d", val);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(val);
        remote()->transact(ATV_AUDIO_SET_FORMAT, data, &reply);
    }
    virtual int ATVCS_matv_audio_get_sound_system()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_audio_get_sound_system");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_GET_SOUND_SYSTEM, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_matv_adjust(int item, int val)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_adjust item=%d, val=%d", item, val);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(item);
        data.writeInt32(val);
        remote()->transact(ATV_ADJUST, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_matv_get_chipdep(int item)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_get_chipdep item=%d", item);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(item);
        remote()->transact(ATV_GET_CHIPDEP, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_matv_set_chipdep(int item, int val)
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_set_chipdep item=%d, val=%d", item, val);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(item);
        data.writeInt32(val);
        remote()->transact(ATV_SET_CHIPDEP, data, &reply);
        return reply.readInt32();
    }
    virtual void ATVCS_matv_register_callback()
    {
        Parcel data, reply;
        SLOGD("ATVCS_matv_register_callback");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(ATV_REG_CALLBACK, data, &reply);
    }
    virtual void registerClient(const sp<IATVCtrlClient>& client)
    {
        Parcel data, reply;
        SLOGD("registerClient");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeStrongBinder(IInterface::asBinder(client));
        remote()->transact(REGISTER_CLIENT, data, &reply);
    }
    virtual void registerClient_FM(const sp<IATVCtrlClient>& client)
    {
        Parcel data, reply;
        SLOGD("registerClient");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeStrongBinder(IInterface::asBinder(client));
        remote()->transact(REGISTER_CLIENT_FM, data, &reply);
    }
    virtual void CLI(char input)
    {
        Parcel data, reply;
        SLOGD("CLI");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(input);
        remote()->transact(ATV_CLI, data, &reply);
    }
    virtual int ATVCS_fm_powerup(void *parm, int len)
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_powerup parm=%p, len=%d", parm, len);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(len);
        data.write(parm, len);
        remote()->transact(FM_POWERUP, data, &reply);
        int ret = reply.readInt32();
        reply.read(parm, len);
        return ret;
    }
    virtual int ATVCS_fm_powerdown()
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_powerdown");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(FM_POWERDOWN, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_fm_getrssi()
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_getrssi");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(FM_GETRSSI, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_fm_tune(void *parm, int len)
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_tune parm=%p, len=%d", parm, len);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(len);
        data.write(parm, len);
        remote()->transact(FM_TUNE, data, &reply);
        int ret = reply.readInt32();
        reply.read(parm, len);
        return ret;
    }
    virtual int ATVCS_fm_seek(void *parm, int len)
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_seek parm=%p, len=%d", parm, len);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(len);
        data.write(parm, len);
        remote()->transact(FM_SEEK, data, &reply);
        int ret = reply.readInt32();
        reply.read(parm, len);
        return ret;
    }
    virtual int ATVCS_fm_scan(void *parm, int len)
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_scan parm=%p, len=%d", parm, len);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(len);
        data.write(parm, len);
        remote()->transact(FM_SCAN, data, &reply);
        int ret = reply.readInt32();
        reply.read(parm, len);
        return ret;
    }
    virtual int ATVCS_fm_mute(int val)
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_mute val=%d", val);
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        data.writeInt32(val);
        remote()->transact(FM_MUTE, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_fm_getchipid()
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_getchipid");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(FM_GETCHIPID, data, &reply);
        return reply.readInt32();
    }
    virtual int ATVCS_fm_isFMPowerUp()
    {
        Parcel data, reply;
        SLOGD("ATVCS_fm_isFMPowerUp");
        data.writeInterfaceToken(IATVCtrlService::getInterfaceDescriptor());
        remote()->transact(FM_ISFMPOWERUP, data, &reply);
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(ATVCtrlService, "android.media.IATVCtrlService");

status_t BnATVCtrlService::onTransact(
    uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    switch (code)
    {
        case ATV_INIT:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_INIT ");
            reply->writeInt32(ATVCS_matv_init());
            return NO_ERROR;
        }
        break;

        case ATV_PS_INIT:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int on = data.readInt32();
            SLOGD("onTransact:ATV_PS_INIT(%d) ", on);
            reply->writeInt32(ATVCS_matv_ps_init(on));
            return NO_ERROR;
        }
        break;

        case ATV_SET_PARAMETERB:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int in = data.readInt32();
            SLOGD("onTransact:ATV_SET_PARAMETERB(%d) ", in);
            reply->writeInt32(ATVCS_matv_set_parameterb(in));
            return NO_ERROR;
        }
        break;

        case ATV_SUSPEND:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int on = data.readInt32();
            SLOGD("onTransact:ATV_SUSPEND(%d)", int(on));
            reply->writeInt32(ATVCS_matv_suspend(on));
            return NO_ERROR;
        }
        break;

        case ATV_SHUTDOWN:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_SHUTDOWN");
            reply->writeInt32(ATVCS_matv_shutdown());
            return NO_ERROR;
        }
        break;

        case ATV_CH_SCAN:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int mode = data.readInt32();
            SLOGD("onTransact:ATV_CH_SCAN (%d)", int(mode));
            ATVCS_matv_chscan(mode);
            return NO_ERROR;
        }
        break;

        case ATV_CH_SCAN_STOP:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_CH_SCAN_STOP ");
            ATVCS_matv_chscan_stop();
            return NO_ERROR;
        }
        break;

        case ATV_GET_CH_TABLE:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int ch  = data.readInt32();
            int len = data.readInt32();
            void *entry = malloc(len);
            data.read(entry, len);
            SLOGD("onTransact:ATV_GET_CH_TABLE ");
            reply->writeInt32(ATVCS_matv_get_chtable(ch, entry, len));
            reply->write(entry, len);
            free(entry);
            return NO_ERROR;
        }
        break;

        case ATV_SET_CH_TABLE:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int ch  = data.readInt32();
            int len = data.readInt32();
            void *entry = malloc(len);
            data.read(entry, len);
            SLOGD("onTransact:ATV_SET_CH_TABLE ");
            reply->writeInt32(ATVCS_matv_set_chtable(ch, entry, len));
            reply->write(entry, len);
            free(entry);
            return NO_ERROR;
        }
        break;

        case ATV_CLEAR_CH_TABLE:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_CLEAR_CH_TABLE ");
            reply->writeInt32(ATVCS_matv_clear_chtable());
            return NO_ERROR;
        }
        break;

        case ATV_CHANGE_CH:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int ch = data.readInt32();
            SLOGD("onTransact:ATV_CHANGE_CH (%d) ", ch);
            ATVCS_matv_change_channel(ch);
            return NO_ERROR;
        }
        break;

        case ATV_SET_COUNTRY:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int country = data.readInt32();
            SLOGD("onTransact:ATV_SET_COUNTRY (%d) ", country);
            ATVCS_matv_set_country(country);
            return NO_ERROR;
        }
        break;

        case ATV_SET_TPARAM:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int mode = data.readInt32();
            SLOGD("onTransact:ATV_SET_TPARAM (%d) ", mode);
            ATVCS_matv_set_tparam(mode);
            return NO_ERROR;
        }
        break;

        case ATV_AUDIO_PLAY:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_AUDIO_PLAY ");
            ATVCS_matv_audio_play();
            return NO_ERROR;
        }
        break;

        case ATV_AUDIO_STOP:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_AUDIO_STOP ");
            ATVCS_matv_audio_stop();
            return NO_ERROR;
        }
        break;

        case ATV_AUDIO_GET_FORMAT:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_AUDIO_GET_FORMAT ");
            reply->writeInt32(ATVCS_matv_audio_get_format());
            return NO_ERROR;
        }
        break;

        case ATV_AUDIO_SET_FORMAT:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int val = data.readInt32();
            SLOGD("onTransact:ATV_AUDIO_SET_FORMAT (%d) ", val);
            ATVCS_matv_audio_set_format(val);
            return NO_ERROR;
        }
        break;

        case ATV_GET_SOUND_SYSTEM:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_GET_SOUND_SYSTEM ");
            reply->writeInt32(ATVCS_matv_audio_get_sound_system());
            return NO_ERROR;
        }
        break;

        case ATV_ADJUST:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int item = data.readInt32();
            int val  = data.readInt32();
            SLOGD("onTransact:ATV_ADJUST (%d)(%d) ", item, val);
            reply->writeInt32(ATVCS_matv_adjust(item, val));
            return NO_ERROR;
        }
        break;

        case ATV_GET_CHIPDEP:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int item = data.readInt32();
            SLOGD("onTransact:ATV_GET_CHIPDEP (%d) ", item);
            reply->writeInt32(ATVCS_matv_get_chipdep(item));
            return NO_ERROR;
        }
        break;

        case ATV_SET_CHIPDEP:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int item = data.readInt32();
            int val = data.readInt32();
            SLOGD("onTransact:ATV_SET_CHIPDEP (%d)(%d) ", item, val);
            reply->writeInt32(ATVCS_matv_set_chipdep(item, val));
            return NO_ERROR;
        }
        break;

        case ATV_REG_CALLBACK:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:ATV_REG_CALLBACK ");
            ATVCS_matv_register_callback();
            return NO_ERROR;
        }
        break;

        case REGISTER_CLIENT:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            sp<IATVCtrlClient> client = interface_cast<IATVCtrlClient>(data.readStrongBinder());
            SLOGD("onTransact:REGISTER_CLIENT");
            registerClient(client);
            return NO_ERROR;
        }
        break;

        case REGISTER_CLIENT_FM:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            sp<IATVCtrlClient> client = interface_cast<IATVCtrlClient>(data.readStrongBinder());
            SLOGD("onTransact:REGISTER_CLIENT_FM");
            registerClient_FM(client);
            return NO_ERROR;
        }
        break;

        case ATV_CLI:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int input = data.readInt32();
            SLOGD("onTransact:ATV_CLI");
            CLI(input);
            return NO_ERROR;
        }
        break;

        case FM_POWERUP:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int len = data.readInt32();
            void *parm = malloc(len);
            data.read(parm, len);
            SLOGD("onTransact:FM_POWERUP ");
            reply->writeInt32(ATVCS_fm_powerup(parm, len));
            reply->write(parm, len);
            free(parm);
            return NO_ERROR;
        }
        break;

        case FM_POWERDOWN:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:FM_POWERDOWN ");
            reply->writeInt32(ATVCS_fm_powerdown());
            return NO_ERROR;
        }
        break;

        case FM_GETRSSI:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:FM_GETRSSI ");
            reply->writeInt32(ATVCS_fm_getrssi());
            return NO_ERROR;
        }
        break;

        case FM_TUNE:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int len = data.readInt32();
            void *parm = malloc(len);
            data.read(parm, len);
            SLOGD("onTransact:FM_TUNE ");
            reply->writeInt32(ATVCS_fm_tune(parm, len));
            reply->write(parm, len);
            free(parm);
            return NO_ERROR;
        }
        break;

        case FM_SEEK:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int len = data.readInt32();
            void *parm = malloc(len);
            data.read(parm, len);
            SLOGD("onTransact:FM_SEEK ");
            reply->writeInt32(ATVCS_fm_seek(parm, len));
            reply->write(parm, len);
            free(parm);
            return NO_ERROR;
        }
        break;

        case FM_SCAN:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int len = data.readInt32();
            void *parm = malloc(len);
            data.read(parm, len);
            SLOGD("onTransact:FM_SCAN ");
            reply->writeInt32(ATVCS_fm_scan(parm, len));
            reply->write(parm, len);
            free(parm);
            return NO_ERROR;
        }
        break;

        case FM_MUTE:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            int val = data.readInt32();
            SLOGD("onTransact:FM_MUTE (%d) ", val);
            reply->writeInt32(ATVCS_fm_mute(val));
            return NO_ERROR;
        }
        break;

        case FM_GETCHIPID:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:FM_GETCHIPID ");
            reply->writeInt32(ATVCS_fm_getchipid());
            return NO_ERROR;
        }
        break;

        case FM_ISFMPOWERUP:
        {
            CHECK_INTERFACE(IATVCtrlService, data, reply);
            SLOGD("onTransact:FM_ISFMPOWERUP ");
            reply->writeInt32(ATVCS_fm_isFMPowerUp());
            return NO_ERROR;
        }
        break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

};
