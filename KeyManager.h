#ifndef __KEY_MANAGER_H__
#define __KEY_MANAGER_H__

#include "CallBackInterface.h"

namespace android {

class KeyManager {
public:
	CREATE_FUNC(KeyManager);
	int init() {
		mCallBackInterface = NULL;
		return 1;
	}
	
	int processKeys(RawEvent *rawEvent) {
		if (mCallBackInterface == NULL) {
			LOGE("[%s][%d] ==> mCallBackInterface = NULL", __FUNCTION__, __LINE__);
			return -1;
		}
		mCallBackInterface->keyProcess(rawEvent);
		return 0;
	}
	
	void registerCallBack(CallBackInterface *callBackInterface) {
		mCallBackInterface = callBackInterface;
		LOGE("[%s][%d] ==> register callbackinterface", __FUNCTION__, __LINE__);
	}
	
private:
	CallBackInterface *mCallBackInterface;
};

};

#endif