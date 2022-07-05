#include "CisExporter.h"

DllExport PREMPLUGENTRY xSDKExport (csSDK_int32 selector, exportStdParms *stdParmsP, void *param1, void *param2)
{
	prMALError result = exportReturn_Unsupported;
	switch (selector)
	{
		case exSelStartup:
			result = exportStartup(stdParmsP, reinterpret_cast<exExporterInfoRec*>(param1));
			break;

		case exSelBeginInstance:
			result = exportBeginInstance(stdParmsP, reinterpret_cast<exExporterInstanceRec*>(param1));
			break;

		case exSelEndInstance:
			result = exportEndInstance(stdParmsP, reinterpret_cast<exExporterInstanceRec*>(param1));
			break;

        case exSelGenerateDefaultParams:
            result = exportGenerateDefaultParams(stdParmsP, reinterpret_cast<exGenerateDefaultParamRec*>(param1));

		case exSelPostProcessParams:
			result = malNoError;
			break;

		case exSelGetParamSummary:
			result = exportGetParamSummary(stdParmsP, reinterpret_cast<exParamSummaryRec*>(param1));
			break;

		case exSelQueryOutputSettings:
			result = exportQueryOutputSettings(stdParmsP, reinterpret_cast<exQueryOutputSettingsRec*>(param1));
			break;

		case exSelParamButton:
			result = malNoError;
			break;

		case exSelValidateParamChanged:
			result = malNoError;
			break;

		case exSelValidateOutputSettings:
			result = malNoError;
			break;

		case exSelExport:
			result = exportExport(stdParmsP, reinterpret_cast<exDoExportRec*>(param1));
			break;
	}
	return result;
}

prMALError exportStartup(exportStdParms *stdParms, exExporterInfoRec *infoRecP)
{
    prMALError result = malNoError;

    // -- default set extension, class & file type
    infoRecP->fileType = CIS_FILETYPE;
    size_t length = wcslen(CIS_FILETYPE_NAME);
	wcscpy_s(infoRecP->fileTypeName, length + 1, CIS_FILETYPE_NAME);
    length = wcslen(CIS_FILETYPE_NAME);
	wcscpy_s(infoRecP->fileTypeDefaultExtension, length + 1, CIS_FILE_EXTENSION);
	infoRecP->classID = CIS_CLASS_ID;

    // -- no audio only & we want a progress bar
	infoRecP->exportReqIndex = 0;
	infoRecP->wantsNoProgressBar = kPrFalse;
	infoRecP->hideInUI = kPrFalse;
	infoRecP->doesNotSupportAudioOnly = kPrTrue;

    // -- video but no audio
	infoRecP->canExportVideo = kPrTrue;
	infoRecP->canExportAudio = kPrFalse;

	// Tell Premiere which headers the exporter was compiled with
	infoRecP->interfaceVersion	= EXPORTMOD_VERSION;

    return result;
}

prMALError exportBeginInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP)
{
    prMALError result = malNoError;
    ExporterSettings* settings = new ExporterSettings(stdParmsP->getSPBasicSuite());

    settings->setPluginId(instanceRecP->exporterPluginID);

    if (!settings->loadSuites()) {
        result = malUnknownError;
    }

    instanceRecP->privateData = reinterpret_cast<void*>(settings);
    return result;
}
prMALError exportEndInstance(exportStdParms *stdParmsP, exExporterInstanceRec *instanceRecP)
{
    prMALError result = malNoError;
    ExporterSettings *settings = reinterpret_cast<ExporterSettings*>(instanceRecP->privateData);
    settings->releaseSuites();
    delete settings;
    return result;
}


prMALError exportGenerateDefaultParams(exportStdParms *stdParmsP, exGenerateDefaultParamRec *paramRecP)
{
    prMALError result = malNoError;
    
    // -- retrieve infos
    ExporterSettings *settings = reinterpret_cast<ExporterSettings*>(paramRecP->privateData);
    // -- create default parameters
    settings->setDefaultParams();
    return result;
}

prMALError exportGetParamSummary(exportStdParms *stdParmsP, exParamSummaryRec* summaryRecP)
{
    prMALError result = malNoError;

    // -- retrieve infos
    ExporterSettings *settings = reinterpret_cast<ExporterSettings*>(summaryRecP->privateData);
    // -- set summary
    settings->setSummary(summaryRecP);
    return result;
}
prMALError exportQueryOutputSettings(exportStdParms *stdParmsP, exQueryOutputSettingsRec *outputSettingsP)
{
    prMALError result = malNoError;
    // -- retrieve infos
    ExporterSettings *settings = reinterpret_cast<ExporterSettings*>(outputSettingsP->privateData);

    // -- set and validate output settings
    if (!settings->setOutputSettings(outputSettingsP)) {
        result = malUnknownError;
    }

    return result;
}
prMALError exportExport(exportStdParms *stdParms, exDoExportRec *exportInfoP)
{
    prMALError result = malNoError;
    // -- retrieve infos
    ExporterSettings *settings = reinterpret_cast<ExporterSettings*>(exportInfoP->privateData);

    // -- export
    if (!settings->exportToCis(exportInfoP)) {
        result = malUnknownError;
    }
    return result;
}