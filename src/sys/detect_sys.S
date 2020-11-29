.set noreorder
#include "pspstub.s"

STUB_START	"detect_sys",0x40090000,0x00010005
	STUB_FUNC	0x00FA5936,GetPspInfo
STUB_END
