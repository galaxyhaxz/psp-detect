#define PSP_SCREEN_WIDTH 480
#define PSP_SCREEN_HEIGHT 272
#define PSP_LINE_SIZE 512

u32 *get_vram_address(int x, int y)
{
	u32 *vram;

	vram = (void *)(0x40000000 | (u32)sceGeEdramGetAddr());
	vram += x;
	vram += y * PSP_LINE_SIZE;

	return vram;
}

u8 bmp_header[] =
{
	0x42, 0x4D, 0x36, 0xFA, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void screenshot_bmp(const char *dst)
{
	int i, j;

	SceUID file = sceIoOpen(dst, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_RDWR, 0777);
	sceIoWrite(file, bmp_header, sizeof(bmp_header));

	u32 *vptr0 = get_vram_address(0, PSP_SCREEN_HEIGHT - 1);

	for(i = 0; i < PSP_SCREEN_HEIGHT; i++)
	{
		u32 *vptr = vptr0;
		u8 buffer[PSP_LINE_SIZE * 3];
		int buffer_index = 0;

		for(j = 0; j < PSP_SCREEN_WIDTH; j++)
		{
			u32 p = *vptr;
			u8 r = p & 0x000000FFL;
			u8 g = (p & 0x0000FF00L) >> 8;
			u8 b = (p & 0x00FF0000L) >> 16;

			buffer[buffer_index] = b;
			buffer_index++;
			buffer[buffer_index] = g;
			buffer_index++;
			buffer[buffer_index] = r;
			buffer_index++;

			vptr++;
		}

		sceIoWrite(file, buffer, PSP_SCREEN_WIDTH * 3);
		vptr0 -= PSP_LINE_SIZE;
	}

	sceIoClose(file);
}