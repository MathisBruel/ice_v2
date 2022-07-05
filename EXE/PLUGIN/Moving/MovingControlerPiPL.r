#include "AEConfig.h"
#include "AE_EffectVers.h"

#ifndef AE_OS_WIN
	#include <AE_General.r>
#endif

resource 'PiPL' (16000) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] The name as it will appear to the user */
		Name {
			"MovingControler"
		},
		/* [3] */
		Category {
			"ICE"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFunc"},
	#else
		CodeWin32X86 {"EntryPointFunc"},
	#endif
#else
	#ifdef AE_OS_MAC
			CodeMachOPowerPC {"EntryPointFunc"},
			CodeMacIntel32 {"EntryPointFunc"},
			CodeMacIntel64 {"EntryPointFunc"},
	#endif
#endif
		/* [6] Application version number */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			PF_PLUG_IN_VERSION,
			PF_PLUG_IN_SUBVERS
		},
		/* [8] */
		AE_Effect_Version {
			0x100001	/* Computed from MAJOR_VERSION, MINOR_VERSION, BUG_VERSION, STAGE_VERSION, BUILD_VERSION */
		},
		/* [9] */
		AE_Effect_Info_Flags {
			0
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
			0x00008114
		},
		AE_Effect_Global_OutFlags_2 {
			0x00040000
		},
		/* [11] */
		AE_Effect_Match_Name {
			"Moving Heads Controler"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};