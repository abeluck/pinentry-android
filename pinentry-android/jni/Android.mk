LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= pinentry-android.c

LOCAL_MODULE:= pinentry-android

LOCAL_STATIC_LIBRARIES := libc

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug tests

#LOCAL_C_INCLUDES +=
#LOCAL_SHARED_LIBRARIES :=
include $(BUILD_EXECUTABLE)
