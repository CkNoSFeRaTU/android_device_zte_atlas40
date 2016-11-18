LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_LDLIBS += -llog -lz
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_SRC_FILES:= taoscal.c

LOCAL_MODULE:= taoscal
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_EXECUTABLE)
