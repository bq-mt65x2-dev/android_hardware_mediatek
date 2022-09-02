LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libatvwrapper
LOCAL_SRC_FILES :=  ATVCtrl.cpp IATVCtrlClient.cpp IATVCtrlService.cpp
LOCAL_SHARED_LIBRARIES := libcutils liblog libutils libbinder libmedia
include $(BUILD_SHARED_LIBRARY)
