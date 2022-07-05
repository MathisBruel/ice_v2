#include <PrSDKMarkerSuite.h>
#include <PrSDKErrorSuite.h>
#include <PrSDKExport.h>
#include "ExporterSettings.h"

#define CIS_FILETYPE_NAME L"Cis"
#define CIS_FILE_EXTENSION L"cis"
#define CIS_FILETYPE 'cis_'
#define CIS_CLASS_ID 'CISE'

// Declare plug-in entry point with C linkage
extern "C" {
    DllExport PREMPLUGENTRY xSDKExport(csSDK_int32 selector, exportStdParms* stdParms, void* param1, void* param2);
}

prMALError exportStartup(exportStdParms *stdParms, exExporterInfoRec *infoRecP);
prMALError exportBeginInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP);
prMALError exportEndInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP);
prMALError exportGenerateDefaultParams(exportStdParms *stdParmsP, exGenerateDefaultParamRec *paramRecP);
prMALError exportGetParamSummary(exportStdParms *stdParmsP, exParamSummaryRec *summaryRecP);
prMALError exportQueryOutputSettings(exportStdParms *stdParmsP, exQueryOutputSettingsRec *outputSettingsP);
prMALError exportExport(exportStdParms *stdParms, exDoExportRec *exportInfoP);