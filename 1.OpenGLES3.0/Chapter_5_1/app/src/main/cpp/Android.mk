MY_CUR_LOCAL_PATH := $(call my-dir)
FRAMEWORK_DIR = ${MY_CUR_LOCAL_PATH}/../../../../../GLPIFramework

SCENE_DIR = ${MY_CUR_LOCAL_PATH}/Scene
GLM_SRC_PATH = $(FRAMEWORK_DIR)/glm
ZLIB_DIR = $(FRAMEWORK_DIR)/zlib
WAVEFRONTOBJ_LIB_PATH = ${FRAMEWORK_DIR}/WaveFrontOBJ
LIB3DS_DIR = $(FRAMEWORK_DIR)/3DSParser

include $(CLEAR_VARS)
include $(FRAMEWORK_DIR)/zlib/Android.mk

LOCAL_PATH := $(MY_CUR_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := glNative

LOCAL_C_INCLUDES := $(GLM_SRC_PATH)/core \
		$(GLM_SRC_PATH)/gtc \
		$(GLM_SRC_PATH)/gtx \
		$(GLM_SRC_PATH)/virtrev \
		$(ZLIB_DIR) \
        $(LIB3DS_DIR) \
		$(FRAMEWORK_DIR) \
		${WAVEFRONTOBJ_LIB_PATH} \
		$(SCENE_DIR)

LOCAL_SRC_FILES := NativeTemplate.cpp \
    $(FRAMEWORK_DIR)/Cache.cpp \
    $(FRAMEWORK_DIR)/ShaderManager.cpp \
	$(FRAMEWORK_DIR)/ProgramManager.cpp \
	$(FRAMEWORK_DIR)/Transform.cpp \
	$(WAVEFRONTOBJ_LIB_PATH)/WaveFrontOBJ.cpp \
    $(SCENE_DIR)/Model.cpp \
    $(SCENE_DIR)/ObjLoader.cpp \
    $(SCENE_DIR)/Renderer.cpp \
    $(SCENE_DIR)/Triangle.cpp \
    $(SCENE_DIR)/Square.cpp \
    $(FRAMEWORK_DIR)/GLutils.cpp \


LOCAL_SHARED_LIBRARIES := zlib
LOCAL_LDLIBS    := -llog -lEGL -lGLESv3

include $(BUILD_SHARED_LIBRARY)