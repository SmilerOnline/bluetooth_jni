#define LOG_TAG "EVENT_HUB"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <utils/Log.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <utils/Vector.h>
#include <utils/KeyedVector.h>
#include "EventHub.h"

#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))

/* this macro computes the number of bytes needed to represent a bit array of the specified size */
#define sizeof_bit_array(bits)  ((bits + 7) / 8)

namespace android {

int EventHub::init() {
	mNextDeviceId = 1;
	iNotifyFd = inotify_init();
	int ret = inotify_add_watch(iNotifyFd, DEVICE_PATH, IN_CREATE | IN_DELETE);
	if (ret < 0) {
		LOGE("[%s][%d] ==> can't register inotify for %s errno = %d (%s)", __FUNCTION__, __LINE__, DEVICE_PATH, errno, strerror(errno));
		return 0;
	}
	return 1;
}

int EventHub::openDeviceLocked(const char *devicePath) {
	char buffer[80];
#if DEBUG_SWITCH
	LOGE("[%s][%d] ==> opening device %s", __FUNCTION__, __LINE__, devicePath);
#endif
	int fd = open(devicePath, O_RDWR);
	if (fd < 0) {
		LOGE("[%s][%d] ==> could not open %s errno = %d (%s)", __FUNCTION__, __LINE__, devicePath, errno, strerror(errno));
		return -1;
	}
	
	InputDeviceIdentifier identifier;
	
	//get device name
	if (ioctl(fd, EVIOCGNAME(sizeof(buffer) - 1), &buffer) < 1) {
		
	} else {
		buffer[sizeof(buffer) - 1] = '\0';
		identifier.name.setTo(buffer);
	}
	
	// check to see if the device is on our ecluded list
	for (size_t i = 0; i < mExcludedDevices.size(); i ++) {
		const String8 &item = mExcludedDevices.itemAt(i);
		if (identifier.name == item) {
			LOGE("[%s][%d] ==> ignoring event id %s driver %s", __FUNCTION__, __LINE__, devicePath, item.string());
			close(fd);
			return -1;
		}
	}
	
	// get device driver version
	int driverVersion;
	if (ioctl(fd, EVIOCGVERSION, &driverVersion)) {
		LOGE("[%s][%d] ==> could not get driver version for %s errno = %d (%s)",
				__FUNCTION__, __LINE__, devicePath, errno, strerror(errno));
		close(fd);
		return -1;
	}
	
	// get device identifier
	struct input_id inputId;
	if (ioctl(fd, EVIOCGID, &inputId)) {
		LOGE("[%s][%d] ==> could not get device input id for %s errno = %d (%s)", 
				__FUNCTION__, __LINE__, devicePath, errno, strerror(errno));
		close(fd);
		return -1;
	}
	
	identifier.bus = inputId.busType;
	identifier.product = inputId.product;
	identifier.vendor = inputId.vendor;
	identifier.version = input.version;
	
	// get device physical location
	if (ioctl(fd, EVIOCGPHYS(sizeof(buffer) - 1), &buffer) < 1) {
		
	} else {
		buffer[siezof[buffer] - 1] = '\0';
		identifier.location.setTo(buffer);
	}
	
	// get device unique id
	if (ioctl(fd, EVIOCGUNIQ(sizeof(buffer) - 1), &buffer) < 1) {
		
	} else {
		buffer[sizeof[buffer] - 1] = '\0';
		identifier.uniqueId.setTo(buffer);
	}
	
	// make file descriptor non-blocking for use with poll()
	
	if (fcntl(fd, F_SETFL, O_NONBLOCK)) {
		LOGE("[%s][%d] ==> making device file descriptor non-blockin erron = %d (%s)", 
				__FUNCTION__, __LINE__, errno, strerror(errno));
		close(fd);
		return -1;
	}
	
	int32_t deviceId = mNextDeviceId++;
	Device *device = new Device(fd, deviceId, String8(devicePaht), identifier);
	
	ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(device->keyBitmask)), device->keyBitmask);
	ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(device->absBitmask)), device->absBitmask);
	ioctl(fd, EVIOCGBIT(EV_REL, sizeof(device->relBitmask)), device->relBitmask);
	ioctl(fd, EVIOCGBIT(EV_SW, sizeof(device->swBitmask)), device->swBitmask);
	ioctl(fd, EVIOCGBIT(EV_LED, sizeof(device->ledBitmask)), device->ledBitmask);
	ioctl(fd, EVIOCGPROP(sizeof(device->propBitmask)), device->propBitmask);
	
	bool haveKeyBoardKeys = containNonZeroByte(device->keyBitmask, 0,
			sizeof_bit_array(BTN_MISC))
			|| containNonZeroByte(device->keyBitmask, sizeof_bit_array(KEY_OK),
					sizeof_bit_array(KEY_MAX + 1));
	bool haveGamepadButtons = containNonZeroByte(device->keyBitmask, sizeof_bit_array(BTN_MISC),
			sizeof_bit_array(BTN_MOUSE));
	if (haveBoardKeys || haveGamepadButtons) {
		device->class |= INPUT_DEVICE_CLASS_KEYBOARD;
	}
	
	//is this a modern multi-touch driver
	if (test_bit(ABS_MT_POSITION_X, device->absBitmask)
			&& test_bit(ABS_MT_POSITION_Y, device->absBitmask)) {
		if (test_bit(BTN_TOUCH, device->keyBitmask) || !haveGamepadButtons) {
			device->class |= INPUT_DEVICE_CLASS_TOUCH | INPUT_DEVICE_CLASS_TOUCH_MT;
		}
	} else if(test_bit(BTN_TOUCH, device->keyBitmask)
			&& test_bit(ABS_X, device->absBitmask)
			&& test_bit(ABS_Y, device->absBitmask)) { //is this an old style single-touch driver
		device->classes |= INPUT_DEVICE_CLASS_TOUCH;
	}
	
	//see if this device is a joystick
	if (haveGamepadButtons) {
		uint32_t assumedClasses = device->class | INPUT_DEVICE_CLASS_JOYSTICK;
		for (int i = 0; i <= ABS_MAX, i++) {
			if (test_bit(i, device->absBitmask)
					&& (getAbsAxisusage(i, assumedClasses) & INPUT_DEVICE_CLASS_JOYSTICK)) {
				device->classes = assumedClass;
			}
		}
	}
	
	LOGE("[%s][%d] ==> new device id = %d fd = %d path = %s name = %s class = 0X%0x"
			, __FUNCTION__, __LINE__, deviceId, devicePath, device->identifier.name.string(),
			device->classes);
	
	mDevice.add(deviceId, device);
	device->next = mOpeningDevices;
	mOpeningDevices = device;
}

uint32_t getAbsAxisUsage(int32_t axis, uint32_t deviceClasses) {
    // Touch devices get dibs on touch-related axes.
    if (deviceClasses & INPUT_DEVICE_CLASS_TOUCH) {
        switch (axis) {
        case ABS_X:
        case ABS_Y:
        case ABS_PRESSURE:
        case ABS_TOOL_WIDTH:
        case ABS_DISTANCE:
        case ABS_TILT_X:
        case ABS_TILT_Y:
        case ABS_MT_SLOT:
        case ABS_MT_TOUCH_MAJOR:
        case ABS_MT_TOUCH_MINOR:
        case ABS_MT_WIDTH_MAJOR:
        case ABS_MT_WIDTH_MINOR:
        case ABS_MT_ORIENTATION:
        case ABS_MT_POSITION_X:
        case ABS_MT_POSITION_Y:
        case ABS_MT_TOOL_TYPE:
        case ABS_MT_BLOB_ID:
        case ABS_MT_TRACKING_ID:
        case ABS_MT_PRESSURE:
        case ABS_MT_DISTANCE:
            return INPUT_DEVICE_CLASS_TOUCH;
        }
    }

    // Joystick devices get the rest.
    return deviceClasses & INPUT_DEVICE_CLASS_JOYSTICK;
}

static bool containNonZeroByte(const uint8_t *array, uint32_t startIndex, uint32_t endIndex) {
	const uint8_t *end = array + endIndex;
	array += startIndex;
	while (array != end) {
		if (*(array++) != 0) {
			return true;
		}
	}
	return false;
}

int EventHub::getEvents(int timeoutMillis, RawEvent *buffer, size_t bufferSize) {
	int ret = 0;
	int max_fd = 0;
	fd_set input;
	
	FD_ZERO(&input);

	if (mDevice.size() == 0) {
		LOGE("[%s][%d] ==> no device");
		goto read_notify;
	}
	
	for (int i = 0; i < mDevices.capacity(); i ++) {
		Device *device = mDevices.valueAt(i);
		max_fd = device->fd > max_fd ? device->fd : max_fd;
	}
	
	max_fd += 1;
	
	ret = select(max_fd, &input, NULL, NULL, NULL);
	
	if (ret < 0) {
		LOGE("[%s][%d] ==>  select failed errno = %d (%s)", __FUNCTION__, __LINE__,
				errno, strerror(errno));
		return -1;
	} else if (0 == ret) {
		LOGE("[%s][%d] ==> time out");
	} else {
		for (int i = 0; i < mDevices.capacity(); i ++) {
			if (FD_ISSET(fd, &input)) {
				readDevice(mDevices.valueAt(i), buffer, bufferSize);
			}
		}
		return 1;
	}
	
read_notify:
	readNotifyLocked();
	
	return 0;
}

int EventHub::readDevice(Device *device, RawEvent *buffer, int bufferSize) {
	struct input_event readBuffer[bufferSize];
	RawEvent *event = buffer;
	
	int32_t readSize = read(device->fd, readBuffer, sizeof(struct input_event) * bufferSize);
	if (readSize == 0 || (readSize < 0 && errno == ENODEV)) {
		LOGE("[%s][%d] ==> could not get event, removed ? (fd = %d size = %d bufferSize = %d \
				errno = %d (%s)", __FUNCTION__, __LINE__, device->fd, readSize, bufferSize, errno, strerror(errno));
		closeDeviceLocked(device);
	} else if (readSize < 0) {
		LOGE("[%s][%d] ==> could not get event error = %d (%s)", __FUNCTION__, __LINE__
				errno, strerror(errno));
	} else if (readSize % sizeof(struct input_event) != 0) {
		LOGE("[%s][%d] ==> could not get event wrong size = %d", __FUNCTION__, __LINE__, readSize);
	} else {
		int count = readSize % sizeof(struct input_event);
		for (int i = 0; i < count; i ++) {
			const struct input_event *iev = readBuffer[i];
			event->deviceId = device->id;
			event->type = iev.type;
			event->scanCode = iev.code;
			event->value = iev.value;
			event->keyCode = 0;
			event->flag = 0;
			event ++;
		}
	}
}

void EventHub::closeDeviceLocked(Device *device) {
	mDevices.removeItem(devie->id);
	device->close();
}

int EventHub::readNotifyLocked() {
	int res;
	char devName[PATH_MAX];
	char *fileName;
	char event_buf[512];
	int event_size;
	int event_pos = 0;
	struct inotify_event *event;
	
#if DEBUG_SWITCH
	LOGE("[%s][%d] ==> readNotify fd = %d", __FUNCTION__, __LINE__, iNotifyFd);
#endif
	res = read(iNotifyFd, event_buf, sizeof(event_buf));
	if (res < (int) sizeof(*event)) {
		if (errno == EINTR) {
			return 0;
		}
		LOGE("[%s][%d] ==> could not get inotify event errno = %d (%s)", __FUNCTION__, __LINE__, errno, strerror(errno));
		return -1;
	}
	
	strcpy(devName, DEVICE_PATH);
	fileName = devName + strlen(devName);
	*fileName ++ ='/';
	
	while (res >= (int)sizeof(*event)) {
		event = (struct inotify_event*) (event_buf + event_pos);
		if (event->len) {
			strcpy(fileName, event->name);
			if (event->mask & IN_CREATE) {
				openDeviceLocked(devName);
			} else {
				closeDeviceLocked(devName);
			}
		}
		event_size = sizeof(*event) + event->len;
		res -= event_size;
		event_pos += event_size;
	}
	
	return 0;
}

EventHub::Device::Device(int fd, int32_t id, const String8 &path, 
		const InputDeviceIdentifier &identifier) :
		fd(fd), id(id), path(path), identifier(identifier) {
	
}

EventHub::Device::~Device() {
	close();
}

void EventHub::Device::close() {
	if (fd >= 0) {
		::close(fd);
		fd = -1;
	}
}

};