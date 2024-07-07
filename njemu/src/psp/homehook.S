	.set noreorder

#include "pspstub.s"

	STUB_START "homehook",0x40090000,0x00030005
	STUB_FUNC  0xDDC19F6B,initHomeButton
	STUB_FUNC  0x40DC34E4,readHomeButton
	STUB_FUNC  0x3337F3E1,readWLANSwitch
	STUB_END
