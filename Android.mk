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

LOCAL_MODULE:= libjnseventhub

include $(BUILD_SHARED_LIBRARY)

