LOCAL_PATH := $(call my-dir)



#OPENCV_LIB_TYPE:=STATIC
include $(CLEAR_VARS)
OPENCV_INSTALL_MODULES := on 
include /home/amankr/android-sdks/OpenCV-2.4.9-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_SHARED_LIBRARIES += libandroid
LOCAL_LDLIBS +=  -llog -ldl -landroid -lEGL 
LOCAL_MODULE    := myjni
LOCAL_SRC_FILES := start.cpp descriptor.cpp hashing.cpp 


include $(BUILD_SHARED_LIBRARY)
