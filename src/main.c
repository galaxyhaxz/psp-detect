#include <pspkernel.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "capture.c"
#include "main.h"

PSP_MODULE_INFO("sceDetective", 0, 1, 1);

#define printf pspDebugScreenPrintf

extern void GetPspInfo(SystemInfo *sys);

typedef struct {
	u32 tachyon;
	u32 baryon;
	char *mobo;
	char *name;
} MoboRev;

MoboRev Revision[] = {
	// Devkit
	{ 0x00140000, 0x00010601, "TMU-001?",     "First" }, /* need confirmation */
	{ 0x00140000, 0x00020601, "TMU-001",      "First" },
	{ 0x00140000, 0x00030601, "TMU-002",      "First" },
	// Fat
	{ 0x00140000, 0x00010600, "TA-079 v1",    "First" }, // Japan-only
	{ 0x00140000, 0x00020600, "TA-079 v2",    "First" }, // Japan-only
	{ 0x00140000, 0x00030600, "TA-079 v3",    "First" }, // historically known as v1
	{ 0x00200000, 0x00030600, "TA-079 v4",    "First" }, // historically known as v2
	{ 0x00200000, 0x00040600, "TA-079 v5",    "First" }, // historically known as v3
	{ 0x00300000, 0x00040600, "TA-081",       "First" },
	{ 0x00400000, 0x00114000, "TA-082",       "Legolas 1" },
	{ 0x00400000, 0x00121000, "TA-086",       "Legolas 2" },
	// Slim
	{ 0x00500000, 0x0022B200, "TA-085 v1",    "Frodo" },
	{ 0x00500000, 0x00234000, "TA-085 v2",    "Frodo" },
	{ 0x00500000, 0x00243000, "TA-088 v1/v2", "Frodo" },
	{ 0x00600000, 0x00243000, "TA-088 v3",    "Frodo" },
	{ 0x00500000, 0x00263100, "TA-090 v1",    "Frodo" }, /* need confirmation */
	// Brite
	{ 0x00600000, 0x00263100, "TA-090 v2",    "Samwise" },
	{ 0x00600000, 0x00285000, "TA-092",       "Samwise" },
	{ 0x00810000, 0x002C4000, "TA-093",       "Samwise VA2" },
	{ 0x00810000, 0x002E4000, "TA-095 v1",    "Samwise VA2" },
	{ 0x00820000, 0x002E4000, "TA-095 v2",    "Samwise VA2" },
	// Go
	{ 0x00720000, 0x00304000, "TA-091",       "Strider" },
	{ 0x00800000, 0x00304000, "TA-094",       "Strider" }, /* need confirmation */
	// Street
	{ 0x00900000, 0x00403000, "TA-096/97",    "Bilbo" }
};

const char *Regions[] = {
	"Japan",
	"United States/Canada",
	"Australia/New Zealand",
	"United Kingdom",
	"Europe",
	"Korea",
	"Hong Kong/Singapore",
	"Taiwan",
	"Russia",
	"China",
	"Mexico"
};

void PrintMobo(u32 t, u32 b)
{
	int i;

	printf("Motherboard: ");

	for(i = 0; i < sizeof(Revision) / sizeof(Revision[0]); i++) {
		if(t == Revision[i].tachyon && b == Revision[i].baryon) {
			printf("%s (%s)\n", Revision[i].mobo, Revision[i].name);
			return;
		}
	}

	printf("Unknown\n");
}

void PrintPspInfo(SystemInfo *sys)
{
	printf("Devkit Version: 0x%08X\n\n", sys->devkit);
	printf("Tachyon: 0x%08X\n", sys->tachyon);
	printf("Baryon:  0x%08X (Timestamp: %s)\n", sys->baryon, sys->timestamp);
	printf("Pommel:  0x%08X\n\n", sys->pommel);

	printf("Shipped Firmware: %s\n\n", sys->firmware);

	PrintMobo(sys->tachyon, sys->baryon);

	printf("Model: ");
	if(sys->tachyon <= 0x00400000) {
		if(sys->baryon == 0x00010601) { // guessed
			printf("DEM-1000 (Development Tool Proto)");
		} else if(sys->baryon == 0x00020601) {
			printf("DTP-T1000 (Development Tool)");
		} else if(sys->baryon == 0x00030601) {
			printf("DTP-H1500/DTP-L1500 (Testing Tool)");
		} else {
			printf("10%02d Fat (01g)", sys->region);
		}
	} else if(sys->tachyon == 0x00500000 || sys->baryon == 0x00243000) {
		printf("20%02d Slim (02g)", sys->region);
	} else if(sys->tachyon == 0x00600000) {
		printf("30%02d Brite (03g)", sys->region);
	} else if(sys->baryon == 0x002C4000) {
		printf("30%02d Brite (04g)", sys->region);
	} else if(sys->baryon == 0x002E4000) {
		printf("30%02d Brite (07g/09g)", sys->region);
	} else if(sys->baryon == 0x00304000) {
		printf("N10%02d Go (05g)", sys->region);
	} else if(sys->baryon == 0x00403000) {
		printf("E10%02d Street (11g)", sys->region);
	} else {
		if(sys->baryon & 1) {
			printf("Unknown (Devkit)");
		} else {
			printf("Unknown");
		}
	}

	printf("\n");

	if(sys->region < 0) {
		printf("Region: N/A\n");
	} else {
		printf("Region: %s\n", Regions[sys->region]);
	}
}

int main(int argc, char *argv[])
{
	SceCtrlData pad;
	SystemInfo sys;

	pspDebugScreenInit();

	SceUID modid = pspSdkLoadStartModule("sys.prx", PSP_MEMORY_PARTITION_KERNEL);
	if(modid < 0) {
		printf("Error 0x%08X starting sys.prx\n", modid);
		sceKernelDelayThread(3*1000*1000);
		sceKernelExitGame();
	}

	memset(&sys, 0, sizeof(sys));
	GetPspInfo(&sys);
	PrintPspInfo(&sys);

	pspDebugScreenSetXY(0, 24);
	pspDebugScreenSetTextColor(0x0000FF00);
	printf("Press X to exit\n");
	printf("Press O to take screenshot\n");
	pspDebugScreenSetTextColor(0x00FFFFFF);

	while(1) {
		sceCtrlReadBufferPositive(&pad, 1);
		if(pad.Buttons & PSP_CTRL_CROSS) {
			break;
		}
		if(pad.Buttons & PSP_CTRL_CIRCLE) {
			screenshot_bmp("ms0:/capture.bmp");
			printf("Screenshot saved to `ms0:/capture.bmp`\n");
			sceKernelDelayThread(5*1000*1000);
			break;
		}
		sceKernelDelayThread(1000);
	}

	sceKernelExitGame();

	return 0;
}
