#include "AE_EffectSuites.h"
#include "AE_EffectCB.h"
#include "AEConfig.h"
#include "AE_EffectCBSuites.h"
#include "AE_Macros.h"
#include "AE_Effect.h"
#include "entry.h"
#include <cstdlib>
#include <string.h>
#include "AEGP_SuiteHandler.h"

#include "MovingHandler.h"
#include "PresetLoader.h"
#include "SizeSequenceHandler.h"

#define MAX_SIZE_EFECT 

typedef struct {
    PresetLoader* ptr;
} PtrPresets;

typedef struct {
	bool flatten;
    MovingHandler* ptr;
} PtrMovingHandler;

typedef struct {
	bool flatten;
    unsigned char flatData[16384];
} FlatMovingHandler;

#ifdef __cplusplus
extern "C" {
#endif

	DllExport	PF_Err 
	EntryPointFunc(	
		PF_Cmd			cmd,
		PF_InData		*in_data,
		PF_OutData		*out_data,
		PF_ParamDef		*params[],
		PF_LayerDef		*output,
		void			*extra) ;

#ifdef __cplusplus
}
#endif