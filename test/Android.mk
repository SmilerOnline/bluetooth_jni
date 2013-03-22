LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        main.cpp

LOCAL_C_INCLUDES:= $(LOCAL_PATH)/../


LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libbinder \
	libevent_hub \
	libtouch_inject

ifeq ($(TARGET_OS),linux)
        LOCAL_CFLAGS += -DXP_UNIX
        #LOCAL_SHARED_LIBRARIES += librt
endif

LOCAL_MODULE:= jnseventhub_test

include $(BUILD_EXECUTABLE)

