#ifndef __BLUETOOTH_EVENT_H__
#define __BLUETOOTH_EVENT_H__

#include <ui/Input.h>
#include <ui/Keyboard.h>
#include <ui/KeyLayoutMap.h>
#include <ui/KeyCharacterMap.h>
#include <ui/VirtualKeyMap.h>
#include <utils/String8.h>
#include <utils/threads.h>
#include <utils/Log.h>
#include <utils/threads.h>
#include <utils/List.h>
#include <utils/Errors.h>
#include <utils/PropertyMap.h>
#include <utils/Vector.h>
#include <utils/KeyedVector.h>
#include <linux/input.h>
#include <sys/epoll.h>
#include "define.h"

namespace android {
/*
 * Input device classes.
 */
enum {
    /* The input device is a keyboard or has buttons. */
    INPUT_DEVICE_CLASS_KEYBOARD      = 0x00000001,

    /* The input device is an alpha-numeric keyboard (not just a dial pad). */
    INPUT_DEVICE_CLASS_ALPHAKEY      = 0x00000002,

    /* The input device is a touchscreen or a touchpad (either single-touch or multi-touch). */
    INPUT_DEVICE_CLASS_TOUCH         = 0x00000004,

    /* The input device is a cursor device such as a trackball or mouse. */
    INPUT_DEVICE_CLASS_CURSOR        = 0x00000008,

    /* The input device is a multi-touch touchscreen. */
    INPUT_DEVICE_CLASS_TOUCH_MT      = 0x00000010,

    /* The input device is a directional pad (implies keyboard, has DPAD keys). */
    INPUT_DEVICE_CLASS_DPAD          = 0x00000020,

    /* The input device is a gamepad (implies keyboard, has BUTTON keys). */
    INPUT_DEVICE_CLASS_GAMEPAD       = 0x00000040,

    /* The input device has switches. */
    INPUT_DEVICE_CLASS_SWITCH        = 0x00000080,

    /* The input device is a joystick (implies gamepad, has joystick absolute axes). */
    INPUT_DEVICE_CLASS_JOYSTICK      = 0x00000100,

    /* The input device is external (not built-in). */
    INPUT_DEVICE_CLASS_EXTERNAL      = 0x80000000,
};

struct RawEvent {
    //nsecs_t when;
    int32_t deviceId;
    int32_t type;
    int32_t scanCode;
    int32_t keyCode;
    int32_t value;
    uint32_t flags;
    uint8_t count;
};

enum TOUCH_TYPE {
	TOUCH_TYPE_SINGLE_POINTER = 0,
	TOUCH_TYPE_MT_POINTER,
	TOUCH_TYPE_RELEASE,
};

struct TouchEvent {
	int32_t x;
	int32_t y;
	int32_t width_major;
	int32_t touch_major;
	int32_t btn_touch;
	int32_t pointer;
	int32_t pressure;
	int32_t touchtype;
};

class EventHub {
public:
	CREATE_FUNC(EventHub);
	int init();
	
	int getEvents(int timeoutMillis, RawEvent *buffer, size_t bufferSize);
	
	struct Device {
		Device *next;
		int fd;
		const int32_t id;
		const String8 path;
		const InputDeviceIdentifier identifier;
		uint32_t classes;			
		uint8_t keyBitmask[(KEY_MAX + 1) / 8];
		uint8_t absBitmask[(ABS_MAX + 1) / 8];
		uint8_t relBitmask[(REL_MAX + 1) / 8];
		uint8_t swBitmask[(SW_MAX + 1) / 8];
		uint8_t ledBitmask[(LED_MAX + 1) / 8];
		uint8_t propBitmask[(INPUT_PROP_MAX + 1) / 8];
	

		Device(int fd, int32_t id, const String8& path, const InputDeviceIdentifier& identifier);
   	        ~Device();

        	void close();
	};
	
	int openDeviceLocked(const char *devicePath);
	void closeDeviceLocked(Device *device);
	Device* getDeviceByPathLocked(const char* devicePath) const;
	Device* getDeviceByClassesLocked(const uint32_t classes) const;
	status_t closeDeviceByPathLocked(const char *devicePath);
	int scanInput();
	int injectTouchData(struct TouchEvent *event, int count);
private:
	
	int readNotifyLocked();
	int readDevice(Device *device, RawEvent *buffer, int bufferSize);
	
	KeyedVector<int32_t, Device *> mDevices;
	Device *mOpeningDevices;
	Device *mClosingDevices;
	
	int iNotifyFd;
	int mNextDeviceId;
	Vector<String8> mExcludedDevices;
};
};

#endif
