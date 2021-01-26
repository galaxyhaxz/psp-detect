#include <pspsdk.h>
#include <pspkernel.h>
#include <pspidstorage.h>
#include <ctype.h>
#include <stdio.h>
#include "../main.h"

PSP_MODULE_INFO("detect_sys", 0x1006, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define sceSysregGetTachyonVersion sceSysreg_driver_E2A5D1EE
#define sceSysconGetBaryonVersion sceSyscon_driver_7EC5A957
#define sceSysconGetTimeStamp sceSyscon_driver_7BCC5EAE
#define sceSysconGetPommelVersion sceSyscon_driver_E7E87741

u32 sceSysregGetTachyonVersion(void);
s32 sceSysconGetTimeStamp(char *timeStamp);
u32 sceSysconGetBaryonVersion(u32 *baryon);
u32 sceSysconGetPommelVersion(u32 *pommel);

static int GetRegion(void)
{
	u8 region[1];
	sceIdStorageLookup(0x100, 0x3D, region, 1);
	if(region[0] == 0x03) return 0; // japan
	else if(region[0] == 0x04) return 1; // north america
	else if(region[0] == 0x09) return 2; // australia/new zealand
	else if(region[0] == 0x07) return 3; // united kingdom
	else if(region[0] == 0x05) return 4; // europe
	else if(region[0] == 0x06) return 5; // korea
	else if(region[0] == 0x0A) return 6; // hong kong/singapore
	else if(region[0] == 0x0B) return 7; // taiwan
	else if(region[0] == 0x0C) return 8; // russia
	else if(region[0] == 0x0D) return 9; // china
	else if(region[0] == 0x08) return 10; // mexico
	//else if(region[0] == 0x01) // dev tool
	//else if(region[0] == 0x02) // test tool
	//else if(region[0] == 0x0E) // av tool
	else return -1; // unknown
}

static void GetShippedFW(char *buf)
{
	sceIdStorageLookup(0x51, 0, buf, 5);
	// TA-079/81 boards do not contain shipped firmware
	// This was added in TA-082 boards to detect downgrading
	if(!isdigit((u8)buf[0]) || buf[1] != '.' || !isdigit((u8)buf[2]) || !isdigit((u8)buf[3])) {
		sprintf(buf, "N/A");
	}
}

void GetPspInfo(SystemInfo *sys)
{
	// user mode API
	sys->devkit = sceKernelDevkitVersion();

	u32 k1 = pspSdkSetK1(0);

	// kernel mode API
	sceSysconGetTimeStamp(sys->timestamp);
	sceSysconGetPommelVersion(&sys->pommel);
	sceSysconGetBaryonVersion(&sys->baryon);
	sys->tachyon = sceSysregGetTachyonVersion();
	GetShippedFW(sys->firmware);
	sys->region = GetRegion();

	pspSdkSetK1(k1);
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop()
{
	return 0;
}
