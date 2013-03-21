#ifndef __DEFINE_H__
#define __DEFINE_H__

#define DEVICE_PATH "/dev/input"

#define CREATE_FUNC(__TYPE__) \
static __TYPE__* create() \
{\
	__TYPE__* pRet = new __TYPE__(); \
	if (pRet && pRet->init()) return pRet; \
	pRet = NULL; \
	return NULL; \
}
	

#endif