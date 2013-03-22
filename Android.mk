LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional eng

LOCAL_SRC_FILES := EventHub.cpp         

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libhardware \
    libhardware_legacy \
    libskia \
    libgui \
    libui

LOCAL_C_INCLUDES := \
    external/skia/include/core


LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libevent_hub

include $(BUILD_SHARED_LIBRARY)
#############################################################################
#LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional eng

LOCAL_SRC_FILES := TouchInject.cpp         

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libhardware \
    libhardware_legacy \
    libskia \
    libgui \
    libui \
    libevent_hub

LOCAL_C_INCLUDES := \
    external/skia/include/core


LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libtouch_inject

include $(BUILD_SHARED_LIBRARY)

#############################################################################
#LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional eng

LOCAL_SRC_FILES := InputAdapter.cpp         

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libhardware \
    libhardware_legacy \
    libskia \
    libgui \
    libui \
    libevent_hub

LOCAL_C_INCLUDES := \
    external/skia/include/core


LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libinput_adapter

include $(BUILD_SHARED_LIBRARY)
