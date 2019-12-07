#include <libusb.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>  
#include <fcntl.h>


int open_chompstick(){

	//chompstick_fd = open(
	return 0;
}

void emit(int fd, int type, int code, int val){
	struct input_event ie;
	ie.type = type;
	ie.code = code;
	ie.value = val;
	ie.time.tv_sec = 0;
	ie.time.tv_usec = 0;

	write(fd, &ie, sizeof(ie));


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

	/*
	dev_numb = libusb_get_device_list(session, &devs);
	
	if (dev_numb < 0){
		printf("Couldn't Get Devices\n");
	}

	device = libusb_open_device_with_vid_pid(session, 39546, 47639);

	if (device == NULL){
		printf("Error, couldn't open device");
		return 1;
	}
	*/

	//struct uinput_setup usetup;

	int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	
	//int fd = open("/dev/input/js0", O_CREAT);
	
	if (fd < 0){
		printf("Error opening file.\n");
	}
	
	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_EVBIT, EV_SYN);
	
	ioctl(fd, UI_SET_KEYBIT, BTN_A);

	ioctl(fd, UI_SET_EVBIT, EV_ABS);
	ioctl(fd, UI_SET_ABSBIT, ABS_X);
	ioctl(fd, UI_SET_ABSBIT, ABS_Y);

	struct uinput_user_dev usetup;

	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0x9A7A;
	usetup.id.product = 0xBA17;
	strcpy(usetup.name, "ChompStick");
	//write(fd, &usetup, sizeof(usetup));
	

	/*usetup.absmax[ABS_X] = 32767;
	usetup.absmin[ABS_X] = -32767;
	usetup.absmax[ABS_Y] = 32767;
	usetup.absmin[ABS_Y] = -32767;
	*/
	//ioctl(fd, UI_DEV_SETUP, &usetup);
	//ioctl(fd, UI_DEV_CREATE);

	write(fd, &usetup, sizeof(usetup));
	ioctl(fd, UI_DEV_CREATE);

	sleep(1);

	
	dev_numb = libusb_get_device_list(session, &devs);

        if (dev_numb < 0){
                printf("Couldn't Get Devices\n");
        }

        device = libusb_open_device_with_vid_pid(session, 39546, 47639);

        if (device == NULL){
                printf("Error, couldn't open device");
                return 1;
        }

	ret_val = libusb_claim_interface(device, 0);
	if (ret_val > 0){
		printf("Couldn't claim device");
	}

	ret_val = libusb_set_interface_alt_setting(device, 0, 0);
	if (ret_val > 0){
		printf("Couldn't set r/w");
	}


	int temp_it = 0;

while(true){

	

	unsigned char buff[2];
	int len = 2;
	ret_val = libusb_interrupt_transfer(device, 0x81, buff, sizeof(buff), &len, 0);

	if (ret_val != 0){
		printf("Error, couldn't transfer\n");
		return 1;
	}


	int val = (int)buff[0];

	//printf("%d", val);
	
	//bool button;
	//int x_ax;
	//int y_ax;
	//printf("\n");
	//for (int j = 0; j > -1; j--){
	//	printf("%d", buff[j]);
	//}

	if (val - 16 > 0){
		//printf("button pressed");
		//button = true;
		val = val - 16;
		emit(fd, EV_KEY, BTN_A, 1);
		emit(fd, EV_SYN, SYN_REPORT, 0);
   		//emit(fd, EV_KEY, BTN_A, 0);
   		//emit(fd, EV_SYN, SYN_REPORT, 0);

	}
	else if (val - 16 < 0){
		//printf("button not pressed");
		//button = false;
		emit(fd, EV_KEY, BTN_A, 0);
  		emit(fd, EV_SYN, SYN_REPORT, 0);
   		//emit(fd, EV_KEY, KEY_SPACE, 0);
   		//emit(fd, EV_SYN, SYN_REPORT, 0);


	}
	if (val - 8 > 0 && val - 12 > 0){
		//printf("right");
		//x_ax = 3;
		val = val - 12;
		emit(fd, EV_KEY, ABS_X, 32767);
		emit(fd, EV_SYN, SYN_REPORT, 0);
	}
	else if(val - 8 > 0 && val - 12 < 0){
		//printf("no x");
		//x_ax = 2;
		val = val - 8;
		emit(fd, EV_KEY, ABS_X, 0);
		emit(fd, EV_SYN, SYN_REPORT, 0);
	}
	else{
		//printf("left");
		//x_ax = 1;
		val = val - 4;
		emit(fd, EV_KEY, ABS_X, -32767);
                emit(fd, EV_SYN, SYN_REPORT, 0);
	}
	if (val == 3){
		//y_ax = 3;
		//printf("up");
		emit(fd, EV_KEY, ABS_Y, 32767);
                emit(fd, EV_SYN, SYN_REPORT, 0);
	}
	else if (val == 2){
		//y_ax = 2;
		//printf("no y");
		emit(fd, EV_KEY, ABS_Y, 0);
                emit(fd, EV_SYN, SYN_REPORT, 0);
	}
	else{
		//y_ax = 1;
		//printf("down");
		emit(fd, EV_KEY, ABS_Y, -32767);
                emit(fd, EV_SYN, SYN_REPORT, 0);
	}
	
	//emit(fd, EV_KEY, KEY_SPACE, button);

	//printf("%d %d %d", button, x_ax, y_ax);
	//int test = val[0];

	temp_it++;
	if (temp_it > 45000){
		break;
	}

}
	libusb_release_interface(device, 0);
	libusb_close(device);
	libusb_exit(session);
	return 0;
}
