/******************************************************************************

	homehook.prx

******************************************************************************/

#include <pspsdk.h>
#include <pspctrl.h>


PSP_MODULE_INFO("homehook", PSP_MODULE_KERNEL, 1, 0);
PSP_MAIN_THREAD_ATTR(0);


/******************************************************************************
	prototypes
******************************************************************************/

int sceCtrl_driver_C4AAD55F(SceCtrlData *pad_data, int count);

#define sceCtrlPeekBufferPositive371	sceCtrl_driver_C4AAD55F


/******************************************************************************
	local variables
******************************************************************************/

static volatile int home_active;
static unsigned int buttons;
static SceUID home_thread;

static int (*__sceCtrlPeekBufferPositive)(SceCtrlData *pad_data, int count);


/******************************************************************************
	functions
******************************************************************************/

static int home_button_thread(SceSize args, void *argp)
{
	SceCtrlData paddata;

	home_active = 1;

	while (home_active)
	{
		if (__sceCtrlPeekBufferPositive)
		{
			(*__sceCtrlPeekBufferPositive)(&paddata, 1);
			buttons = paddata.Buttons;
		}

		sceKernelDelayThread(1000000/60);
	}

	sceKernelExitDeleteThread(0);

	return 0;
}


void initHomeButton(int devkit_version)
{
	if (devkit_version < 0x03070110)
		__sceCtrlPeekBufferPositive = sceCtrlPeekBufferPositive;
	else
		__sceCtrlPeekBufferPositive = sceCtrlPeekBufferPositive371;
}


unsigned int readHomeButton(void)
{
	return buttons & PSP_CTRL_HOME;
}


unsigned int readWLANSwitch(void)
{
	return buttons & PSP_CTRL_WLAN_UP;
}


int module_start(SceSize args, void *argp)
{
	__sceCtrlPeekBufferPositive = NULL;

	buttons = 0;
	home_thread = sceKernelCreateThread(
								"Home Button Thread",
								home_button_thread,
								0x11,
								0x200,
								0,
								NULL);

	if (home_thread >= 0)
		sceKernelStartThread(home_thread, 0, 0);

	return 0;
}


int module_stop(void)
{
	if (home_thread >= 0)
	{
		home_active = 0;
		sceKernelWaitThreadEnd(home_thread, NULL);
	}
	return 0;
}
