/*
 * Bluetooth Vendor Library for MTK's libbluetoothdrv blob
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "libbt_vendor_mtk"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <utils/Log.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <bt_vendor_lib.h>
#include <bt_hci_bdroid.h>
#include <hardware/bluetooth.h>

// Keep this struct aligned with libnvram.
typedef struct ap_nvram_btradio_struct {
    unsigned char addr[6];            // BT address
    unsigned char Voice[2];           // Voice setting for SCO connection
    unsigned char Codec[4];           // PCM codec setting
    unsigned char Radio[6];           // RF configuration
    unsigned char Sleep[7];           // Sleep mode configuration
    unsigned char BtFTR[2];           // Other feature setting
    unsigned char TxPWOffset[3];      // TX power channel offset compensation
    unsigned char CoexAdjust[6];      // BT/WIFI coexistence performance adjustment
    unsigned char Reserved1[2];       // Reserved
    unsigned char Reserved2[2];       // Reserved
    unsigned char Reserved3[4];       // Reserved
    unsigned char Reserved4[4];       // Reserved
    unsigned char Reserved5[8];       // Reserved
    unsigned char Reserved6[8];       // Reserved
} ap_nvram_btradio_struct;

/**
 * TODO: check/fix this value. does this make sense for MTK? It is taken from TI
 * Low power mode: default transport idle timer
 */
#define WL_DEFAULT_LPM_IDLE_TIMEOUT 200

static void *mtklib_handle = NULL;
typedef int (*ENABLE)(int flag, void *func_cb);
typedef int (*DISABLE)(int bt_fd);

unsigned int hci_tty_fd = -1;

ENABLE mtk_bt_enable = NULL;
DISABLE mtk_bt_disable = NULL;

bt_vendor_callbacks_t *bt_vendor_cbacks = NULL;
void hw_config_cback(HC_BT_HDR *p_evt_buf);

/*******************************************************************************
 *
 * Function         hw_config_cback
 *
 * Description      Callback function for controller configuration
 *
 * Returns          None
 *
 * *******************************************************************************/
void hw_config_cback(HC_BT_HDR *p_evt_buf)
{
  ALOGI("hw_config_cback");
}

int mtk_init(const bt_vendor_callbacks_t* p_cb, unsigned char *local_bdaddr) {
  const char *dlerrors;

  ALOGI("libbt-vendor init for MTK blob");

  if (p_cb == NULL)
    {
      ALOGE("init failed with no user callbacks!");
      return -1;
    }

  bt_vendor_cbacks = (bt_vendor_callbacks_t *) p_cb;

  dlerror();

  mtklib_handle = dlopen("libbluetoothdrv.so", RTLD_LAZY);
  if (! mtklib_handle) {
    ALOGE("Failed to open libbluetoothdrv library");
    return (int)mtklib_handle;
  }

  mtk_bt_enable = dlsym(mtklib_handle, "mtk_bt_enable");
  mtk_bt_disable = dlsym(mtklib_handle, "mtk_bt_disable");

  if ((dlerrors = dlerror()) != NULL){
    ALOGE("Errors while opening symbols from mtk blob");
    dlclose(mtklib_handle);
  }
  return 0;
}

int mtk_gen_new_mac() {
    unsigned char BtAddr[6];
    struct ap_nvram_btradio_struct btradioStruct;

    srand(time(NULL) + getpid());

    for (int i = 0; i < 6; i++) {
        btradioStruct.addr[i] = rand() % 256;
    }

    // Our BT HAL looks for the MAC address inside
    // this file.
    int fd = open("/data/BT_Addr", O_CREAT|O_WRONLY);
    if (fd < 0) {
        if (write(fd, &btradioStruct, 1*sizeof(ap_nvram_btradio_struct)) != -1) {
             return 0;
        }
    }

    // We shouldn't be here since that means we were
    // unable to write the new MAC.
    return -1;
}

void mtk_cleanup(void) {
  ALOGI("vendor cleanup");
  bt_vendor_cbacks = NULL;

  if (mtklib_handle) {
    dlclose(mtklib_handle);
  }
}

int mtk_open(void **param) {
  int (*fd_array)[] = (int (*)[]) param;
  int fd, idx;
  fd = mtk_bt_enable(0, NULL);
  if (fd < 0) {
    // We failed? This usually means there was no MAC
    // address set at this point. It's okay! Generate
    // a new one and retry again!
    ALOGW("bt_enable() failed, not dying yet!");
    if (mtk_gen_new_mac() < 0) {
        fd = mtk_bt_enable(0, NULL);
        // We failed? If the MAC wasn't the problem
        // there's nothing much we can do. Bail out.
        if (fd < 0) {
            ALOGE("Can't open mtk fd");
            return -1;
        }
        ALOGI("Saved you by generating a new MAC!");
        return 1;
    }
    ALOGE("Unable to generate a new MAC");
    return -1;
  }
  for (idx = 0; idx < CH_MAX; idx++)
    (*fd_array)[idx] = fd;

  hci_tty_fd = fd; /* for userial_close op */
  return 1;  /* CMD/EVT/ACL on same fd */
}

int mtk_close() {
  if (hci_tty_fd == (unsigned int) -1)
    return -1;
  return mtk_bt_disable(hci_tty_fd);
}

int mtk_op(bt_vendor_opcode_t opcode, void **param) {
  int ret = 0;

  switch(opcode)
    {
    case BT_VND_OP_POWER_CTRL:
      break;
    case BT_VND_OP_SCO_CFG:
      break;
    case BT_VND_OP_GET_LPM_IDLE_TIMEOUT:
      *((uint32_t *) param) = WL_DEFAULT_LPM_IDLE_TIMEOUT;
      break;
    case BT_VND_OP_LPM_SET_MODE:
      break;
    case BT_VND_OP_LPM_WAKE_SET_STATE:
      break;
    case BT_VND_OP_USERIAL_OPEN:
      ret = mtk_open(param);
      break;
    case BT_VND_OP_USERIAL_CLOSE:
      ret = mtk_close();
      break;
      /* Since new stack expects fwcfg_cb we are returning SUCCESS here
       * in actual, firmware download is already happened when /dev/hci_tty
       * opened.
       */
    case BT_VND_OP_FW_CFG:
      bt_vendor_cbacks->fwcfg_cb(BT_VND_OP_RESULT_SUCCESS);
      break;
    case BT_VND_OP_EPILOG:
      bt_vendor_cbacks->epilog_cb(BT_VND_OP_RESULT_SUCCESS);
      break;
    default:
      ALOGW("Unknown opcode: %d", opcode);
      break;
    }

  return ret;
}
const bt_vendor_interface_t BLUETOOTH_VENDOR_LIB_INTERFACE  = {
  .init = mtk_init,
  .op = mtk_op,
  .cleanup = mtk_cleanup,
};

int main()
{
  return 0;
}
