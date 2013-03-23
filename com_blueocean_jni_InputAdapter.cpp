#define LOG_TAG "JN_INPUT_ADAPTER"

#ifdef BUILD_USING_NDK
#include <android/log.h>
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)
#else
#include <utils/Log.h>
#endif

#include <assert.h>
#include <jni.h>
#include "InputAdapter.h"
#include "CallBackInterface.h"

#ifndef BUILD_USING_NDK
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#endif

struct fields_t {
	jmethodID onInputAdapterKeyDown;
	jmethodID onInputAdapterKeyUp;
	jmethodID onInputAdapterJoystickChange;
};

static fields_t inputCallBackField;

typedef struct tagJNiGlobale_t {
	JNIEnv *env;
	JavaVM *jvm;
} stJniGlobal_t;

static stJniGlobal_t sg_jni_global;
static jclass g_com_blueocean_jni_InputAdapter_class;

static void init_jni_global() {
	memset(&sg_jni_global, 0x00, sizeof(stJniGlobal_t));
}


#ifdef BUILD_USING_NDK
class AndroidRuntime {
public:
	static JNIEnv* getJNIEnv() {
		if (NULL != sg_jni_global.jvm && NULL == sg_jni_global.env) {
			sg_jni_global.jvm->GetEnv((void **) &sg_jni_global.env,
					JNI_VERSION_1_4);
		}
		return sg_jni_global.env;
	}
};
#else
using namespace android;
#endif

class InputAdapterCallBack : public CallBackInterface {
public:
	int keyProcess(const RawEvent *rawEvent) {
		int scanCode = rawEvent->scanCode;
		int value = rawEvent->value;
		if (value == 1) {
			doOnKeyDown(scanCode, value);
		} else {
			doOnKeyUp(scanCode, value);
		}
		return 1; 
	};
	
	int joystickProcess(const RawEvent *rawEvent) { 
		int scanCode = rawEvent->scanCode;
		int value = rawEvent->value;
		doOnJoystickDataChange(scanCode, value);
		return 1; 
	};
};


static void doOnKeyDown(int scanCode, int value) {
	JNIEnv *_env = AndroidRuntime::getJNIEnv();
	_env->CallStaticVoidMethod(g_com_blueocean_jni_InputAdapter_class, 
			onInputAdapterKeyDown, scanCode, value);
}

static void doOnKeyUp(int scanCode, int value) {
	JNIEnv *_env = AndroidRuntime::getJNIEnv();
	_env->CallStaticVoidMethod(g_com_blueocean_jni_InputAdapter_class, 
			onInputAdapterKeyUp, scanCode, value);
}

static void doOnJoystickDataChange(int scanCode, value) {
	JNIEnv *_env = AndroidRuntime::getJNIEnv();
	_env->CallStaticVoidMethod(g_com_blueocean_jni_InputAdapter_class, 
			onInputAdapterJoystickChange, scanCode, value);
}


static sp<InputAdapter> mInputAdapter = NULL;
static sp<InputAdapterCallBack> mInputAdapterCallBack;

static jbool com_blueocean_jni_InputAdapter_init(JNIEnv *env, jclass clazz) {
	jclass thiz = env->FindClass("com/blueocean/jni/InputAdapter");
	if (thiz == NULL) {
		LOGE("[%s][%d] ==> can't find class com_blueocean_jni_InputAdapter", __FUNCTION__, __LINE__);
		return J_FALSE;
	}
	
	jclass jclazz = env->GetObjectClass(clazz);
	if (jclazz == NULL) {
		g_com_blueocean_jni_InputAdapter_class = (jclass) 0;
		LOGE("[%s][%d] ==> can't find class com_blueocean_jni_InputAdpater", __FUNCTION__, __LINE__);
		return J_FALSE;
	}
	
	g_com_blueocean_jni_InputAdapter_class = env->NewGlobalRef(jclazz);
	
	inputCallBackField.onInputAdapterKeyDown = env->GetStaticMethodID(thiz, "onInputAdapterKeyDown", "(II)V");
	if (inputCallBackField.onInputAdapterKeyDown == NULL) {
		LOGE("[%s][%d] ==> can't get onInputAdapterKeyDown from com.blueocean.jni.InputCallBack class file", __FUNCITON__, __LINE__);
		return J_TRUE;
	}
	inputCallBackField.onInputAdapterKeyUp = env->GetStaticMethodID(thiz, "onInputAdapterKeyUp", "(II)V");
	if (inputCallBackField.onInputAdapterKeyUp == NULL) {
		LOGE("[%s][%d] ==> can't get onInputAdapterKeyUp from com.blueocean.jni.InputCallBack class file", __FUNCITON__, __LINE__);
		return J_FALSE;
	}
	inputCallBackField.onInputAdapterJoystickChange = env->GetStaticMethodID(thiz, "onInputAdapterJoystickChange", "(II)V");
	if (inputCallBackField.onInputAdapterJoystickChange == NULL) {
		LOGE("[%s][%d] ==> can't get onInputAdapterJoystickChange from com.blueocean.jni.InputCallBack class file", __FUNCITON__, __LINE__);
		return J_FALSE;
	}
	
	mInputAdapter = InputAdapter::create();
	mInputAdapterCallBack = new InputAdapterCallBack();
	mInputAdapter->getKeyManager()->registerCallBack(mInputAdapterCallBack);
	mInputAdapter->getJoystick()->registerCallBack(mInputAdapterCallBack);
	
	return J_TRUE;
}

static jbool com_blueocean_jni_InputAdapter_start(JNIEnv *env, jclass clazz) {
	if (mInputAdapter == NULL) {
		LOGE("[%s][%d] ==> mInputAdapter is NULL", __FUNCTION__, __LINE__);
		return J_FALSE;
	}
	mInputAdapter->start();
	return J_TRUE;
}

static jbool com_blueocean_jni_InputAdapter_stop(JNIEnv *env, jclass clazz) {
	if (mInputAdapter == NULL) {
		LOGE("[%s][%d] ==> mInputAdapter is NULL", __FUNCTION__, __LINE__);
		return J_FALSE;
	}
	
	mInputAdapter->stop();
	return J_TRUE;
}

