#include <libusb.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdbool.h>


int chompstick_fd = -1;

int open_chompstick(){

	//chompstick_fd = open(
	return 0;
}

//int read_chompstick_event(

int main(){

	libusb_device **devs;
	libusb_context *session;
	libusb_device_handle *device;

	int ret_val;
	int dev_numb;
	ret_val = libusb_init(&session);
	if (ret_val < 0){
		printf("Init Error\n");
		return 1;
	}
	
	libusb_set_debug(session, 3);

	dev_numb = libusb_get_device_list(session, &devs);
	
	if (dev_numb < 0){
		printf("Couldn't Get Devices\n");
	}

	device = libusb_open_device_with_vid_pid(session, 39546, 47639);

	if (device == NULL){
		printf("Error, couldn't open device");
		return 1;
	}

	struct uinput_setup usetup;

	int fd = open("/dev/input/js0", O_WRONLY | O_NONBLOCK);
	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_KEYBIT, KEY_SPACE);

	ioctl(fd, UI_SET_EVBIT, EV_ABS);
	ioctl(fd, UI_SET_ABSBIT, ABS_X);
	ioctl(fd, UI_SET_ABSBIT, ABS_Y);

	memset(&usetup, 0, sizeof(usetup));


	

	unsigned char buff[2];
	int len = 2;
	ret_val = libusb_interrupt_transfer(device, 0x81, buff, sizeof(buff), &len, 0);

	if (ret_val != 0){
		printf("Error, couldn't transfer\n");
		return 1;
	}


	int val = (int)buff[0];

	printf("%d", val);
	
	bool button;
	int x_ax;
	int y_ax;
	//printf("\n");
	//for (int j = 0; j > -1; j--){
	//	printf("%d", buff[j]);
	//}

	if (val - 16 > 0){
		//printf("button pressed");
		button = true;
		val = val - 16;
	}
	else if (val - 16 < 0){
		//printf("button not pressed");
		button = false;
	}
	if (val - 8 > 0 && val - 12 > 0){
		//printf("right");
		x_ax = 3;
		val = val - 12;
	}
	else if(val - 8 > 0 && val - 12 < 0){
		//printf("no x");
		x_ax = 2;
		val = val - 8;
	}
	else{
		//printf("left");
		x_ax = 1;
		val = val - 4;
	}
	if (val == 3){
		y_ax = 3;
		//printf("up");
	}
	else if (val == 2){
		y_ax = 2;
		//printf("no y");
	}
	else{
		y_ax = 1;
		//printf("down");
	}
	
	printf("%d %d %d", button, x_ax, y_ax);
	//int test = val[0];

	libusb_release_interface(device, 0);
	libusb_close(device);
	libusb_exit(session);
	return 0;
}
