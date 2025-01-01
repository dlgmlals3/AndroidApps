MY_CUR_LOCAL_PATH := $(call my-dir)
FRAMEWORK_DIR = ${MY_CUR_LOCAL_PATH}/../../../../../GLPIFramework

SCENE_DIR = ${MY_CUR_LOCAL_PATH}/Scene
GLM_SRC_PATH = $(FRAMEWORK_DIR)/glm
ZLIB_DIR = $(FRAMEWORK_DIR)/zlib
TEXTURE_LIB_PATH= $(FRAMEWORK_DIR)/ImageBuffer
WAVEFRONTOBJ_LIB_PATH = ${FRAMEWORK_DIR}/WaveFrontOBJ
FONT_PATH= $(FRAMEWORK_DIR)/Font

include $(CLEAR_VARS)
include $(FRAMEWORK_DIR)/libpng/Android.mk
include $(FRAMEWORK_DIR)/zlib/Android.mk
include $(FRAMEWORK_DIR)/Font/FreeType/Android.mk
LOCAL_PATH := $(MY_CUR_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := glNative

LOCAL_C_INCLUDES := $(GLM_SRC_PATH)/core \
		$(GLM_SRC_PATH)/gtc \
		$(GLM_SRC_PATH)/gtx \
		$(GLM_SRC_PATH)/virtrev \
		$(ZLIB_DIR) \
		$(FRAMEWORK_DIR) \
		$(SCENE_DIR) \
		$(FREEIMAGELIB_DIR)/Source \
		$(TEXTURE_LIB_PATH) \
		$(WAVEFRONTOBJ_LIB_PATH)

LOCAL_C_INCLUDES += $(FONT_PATH)/FreeType/include

LOCAL_SRC_FILES := $(FRAMEWORK_DIR)/GLutils.cpp \
	$(FRAMEWORK_DIR)/Cache.cpp \
	$(FRAMEWORK_DIR)/ShaderManager.cpp \
	$(FRAMEWORK_DIR)/ProgramManager.cpp \
	$(FRAMEWORK_DIR)/Transform.cpp \
	$(WAVEFRONTOBJ_LIB_PATH)/WaveFrontOBJ.cpp \
	$(SCENE_DIR)/Renderer.cpp \
	$(SCENE_DIR)/Model.cpp \
	$(TEXTURE_LIB_PATH)/Image.cpp \
	$(TEXTURE_LIB_PATH)/PngImage.cpp \
	$(SCENE_DIR)/SimpleTexture.cpp \
    NativeTemplate.cpp

LOCAL_SRC_FILES += \
    $(SCENE_DIR)/FontGenerator.cpp \
	$(SCENE_DIR)/Font.cpp \
	$(SCENE_DIR)/FontSample.cpp
	
LOCAL_SHARED_LIBRARIES := zlib GLPipng
LOCAL_SHARED_LIBRARIES += GLPift2

LOCAL_LDLIBS    := -llog -lGLESv3
LOCAL_CFLAGS := -O3 -mno-thumb -Wno-write-strings

include $(BUILD_SHARED_LIBRARY)