MY_CUR_LOCAL_PATH := $(call my-dir)
FRAMEWORK_DIR = ${MY_CUR_LOCAL_PATH}/../../../../../GLPIFramework

SCENE_DIR = ${MY_CUR_LOCAL_PATH}/Scene
MISC_DIR = ${MY_CUR_LOCAL_PATH}/Scene/misc
GLM_SRC_PATH = $(FRAMEWORK_DIR)/glm
ZLIB_DIR = $(FRAMEWORK_DIR)/zlib
TEXTURE_LIB_PATH= $(FRAMEWORK_DIR)/ImageBuffer
WAVEFRONTOBJ_LIB_PATH = ${FRAMEWORK_DIR}/WaveFrontOBJ
LIB3DS_DIR = $(FRAMEWORK_DIR)/3DSParser
FONT_PATH= $(FRAMEWORK_DIR)/Font

include $(CLEAR_VARS)
include $(FRAMEWORK_DIR)/libpng/Android.mk
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
        $(WAVEFRONTOBJ_LIB_PATH) \
		$(FRAMEWORK_DIR) \
		$(FRAMEWORK_DIR)/Geometry \
		$(SCENE_DIR) \
        $(MISC_DIR) \
		$(TEXTURE_LIB_PATH)
LOCAL_C_INCLUDES += $(FONT_PATH)/FreeType/include

LOCAL_SRC_FILES := \
    	$(FRAMEWORK_DIR)/GLutils.cpp \
	$(FRAMEWORK_DIR)/Cache.cpp \
    	$(FRAMEWORK_DIR)/ShaderManager.cpp \
    	$(FRAMEWORK_DIR)/ProgramManager.cpp \
    	$(FRAMEWORK_DIR)/Transform.cpp \
	$(FRAMEWORK_DIR)/Light.cpp \
	$(FRAMEWORK_DIR)/Material.cpp \
	$(FRAMEWORK_DIR)/Geometry/Ray.cpp \
	$(SCENE_DIR)/RendererEx.cpp \
	$(SCENE_DIR)/Object.cpp \
	$(SCENE_DIR)/ModelEx.cpp \
	$(SCENE_DIR)/Scene.cpp \
	$(SCENE_DIR)/Event.cpp \
    	$(TEXTURE_LIB_PATH)/Image.cpp \
    	$(TEXTURE_LIB_PATH)/PngImage.cpp \
    	$(WAVEFRONTOBJ_LIB_PATH)/WaveFrontOBJ.cpp \
    	$(SCENE_DIR)/ObjLoader.cpp \
    	NativeTemplate.cpp

LOCAL_SHARED_LIBRARIES := zlib GLPipng

LOCAL_LDLIBS    := -llog -lEGL -lGLESv3
LOCAL_CFLAGS := -O3 -mno-thumb -Wno-write-strings

include $(BUILD_SHARED_LIBRARY)