#ifndef __MAIN_H__
#define __MAIN_H__

typedef struct {
	u32 devkit;
	u32 tachyon;
	u32 baryon;
	u32 pommel;
	int region;
	char timestamp[16];
	char firmware[8];
} SystemInfo;

#endif /* __MAIN_H__ */
