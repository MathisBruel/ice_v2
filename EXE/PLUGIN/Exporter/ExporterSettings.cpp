#include "ExporterSettings.h"

void renderAndTrySaveFrame(ExporterSettings* exporter, long frameNb, csSDK_int32 rowbytes, char* frameBufferP)
{
    char* frameReversed = nullptr;
    frameReversed = new char[exporter->widthF * exporter->heightF * 4];

    // -- remove padding between pixels
	if (rowbytes != exporter->widthF * 4) {
        ExporterSettings::removeRowPadding(frameBufferP, frameReversed, rowbytes, 4, exporter->widthF, exporter->heightF);
        rowbytes = exporter->widthF * 4;
    }
    else {
        ExporterSettings::reverseImage(frameBufferP, frameReversed, rowbytes, 4, exporter->widthF, exporter->heightF);
    }

    // -- add image to cis and try to save it
    exporter->cis->addImageAsync(frameNb, frameReversed);

    if (exporter->cis->tryLockMutexSaving()) {
        exporter->cis->trySave();
        exporter->cis->unlockMutexSaving();
    }
}

ExporterSettings::ExporterSettings(SPBasicSuite *mSpBasic)
{
    this->spBasic = mSpBasic;
    this->videoRenderId = -1;
}

ExporterSettings::~ExporterSettings()
{

}

bool ExporterSettings::loadSuites()
{
    if (spBasic == nullptr) {
        return false;
    }

    SPErr spError;

    spError = spBasic->AcquireSuite(kPrSDKExportParamSuite, kPrSDKExportParamSuiteVersion,
        const_cast<const void**>(reinterpret_cast<void**>(&exportParamSuite)));

    spError = spBasic->AcquireSuite(kPrSDKExportInfoSuite, kPrSDKExportInfoSuiteVersion,
        const_cast<const void**>(reinterpret_cast<void**>(&exportInfoSuite)));

    spError = spBasic->AcquireSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion,
        const_cast<const void**>(reinterpret_cast<void**>(&exportFileSuite)));

    spError = spBasic->AcquireSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion,
        const_cast<const void**>(reinterpret_cast<void**>(&exportProgressSuite)));

    spError = spBasic->AcquireSuite(kPrSDKSequenceRenderSuite, kPrSDKSequenceRenderSuiteVersion,
        const_cast<const void**>(reinterpret_cast<void**>(&sequenceRenderSuite)));

    spError = spBasic->AcquireSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion, 
        const_cast<const void**>(reinterpret_cast<void**>(&timeSuite)));

    spError = spBasic->AcquireSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion, 
        const_cast<const void**>(reinterpret_cast<void**>(&ppixSuite)));

    spError = spBasic->AcquireSuite(kPrSDKExporterUtilitySuite, kPrSDKExporterUtilitySuiteVersion, 
        const_cast<const void**>(reinterpret_cast<void**>(&exportUtilitySuite)));

    return true;
}

void ExporterSettings::releaseSuites()
{
    if(spBasic != nullptr) {
        spBasic->ReleaseSuite(kPrSDKExportParamSuite, kPrSDKExportParamSuiteVersion);
        spBasic->ReleaseSuite(kPrSDKExportInfoSuite, kPrSDKExportInfoSuiteVersion);
        spBasic->ReleaseSuite(kPrSDKExportFileSuite, kPrSDKExportFileSuiteVersion);
        spBasic->ReleaseSuite(kPrSDKExportProgressSuite, kPrSDKExportProgressSuiteVersion);
        spBasic->ReleaseSuite(kPrSDKSequenceRenderSuite, kPrSDKSequenceRenderSuiteVersion);
        spBasic->ReleaseSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion);
        spBasic->ReleaseSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion);
        spBasic->ReleaseSuite(kPrSDKExporterUtilitySuite, kPrSDKExporterUtilitySuiteVersion);
    }
}

void ExporterSettings::setDefaultParams()
{
    // -- create group
    csSDK_int32 groupIdx;
    exportParamSuite->AddMultiGroup(pluginId, &groupIdx);

    // -- create group ICE
    exportParamSuite->AddParamGroup(pluginId, groupIdx, ADBETopParamGroup, "ICE_ID", L"ICE", kPrFalse, kPrFalse, kPrFalse);

    // -- create Version 
    exParamValues versionValues;
    versionValues.disabled = kPrFalse;
    versionValues.hidden = kPrFalse;
    versionValues.value.intValue = 1;
    versionValues.rangeMin.intValue = 1;
    versionValues.rangeMax.intValue = 2;
    exNewParamInfo versionParam;
    strcpy_s(versionParam.identifier, sizeof(versionParam.identifier), "VERSION_ID");
    versionParam.paramType = exParamType_int;
    versionParam.flags = exParamFlag_none;
    versionParam.paramValues = versionValues;

    // -- add LUT component to group and set its name
    exportParamSuite->AddParam(pluginId, groupIdx, "ICE_ID", &versionParam);
    exportParamSuite->SetParamName(pluginId, 0, "VERSION_ID", L"Version");
}

bool ExporterSettings::setOutputSettings(exQueryOutputSettingsRec *outputSettingsP)
{
    PrParam width;
    PrParam height;
    PrParam PARNum;
    PrParam PARDen;
    PrParam frameRate;
    exParamValues fieldType;

    // -- check no audio
    if (outputSettingsP->inExportAudio != 0) {
        return false;
    }

    // -- check video
    if (outputSettingsP->inExportVideo == 0) {
        return false;
    }

    // -- get parameters
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoWidth, &width);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoHeight, &height);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoFrameRate, &frameRate);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_PixelAspectNumerator, &PARNum);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_PixelAspectDenominator, &PARDen);
    exportParamSuite->GetParamValue(pluginId, 0, ADBEVideoFieldType, &fieldType);

    // -- set parameters
    width.mInt32 = 720;
    height.mInt32 = 576;

    // -- set output settings
    outputSettingsP->outVideoWidth = width.mInt32;
    outputSettingsP->outVideoHeight = height.mInt32;
    outputSettingsP->outVideoFrameRate = frameRate.mInt64;
    outputSettingsP->outVideoAspectNum = PARNum.mInt32;
    outputSettingsP->outVideoAspectDen = PARDen.mInt32;
    outputSettingsP->outVideoFieldType = fieldType.value.intValue;

    // Calculate bitrate
	PrTime ticksPerSecond = 0;
	csSDK_uint32 videoBitrate = 0;

	timeSuite->GetTicksPerSecond(&ticksPerSecond);
	float fps = static_cast<float>(ticksPerSecond) / frameRate.mInt64;
    // -- we assume that format of pixel is 8 bit so it gives RGBA of 4 bytes
	videoBitrate = static_cast<csSDK_uint32>(width.mInt32 * height.mInt32 * 4 * fps);

    // -- conversion in kbps
    outputSettingsP->outBitratePerSecond = videoBitrate * 8 / 1000;

    return true;
}

void ExporterSettings::setSummary(exParamSummaryRec *summaryRecP)
{
    // -- audio : no set
    size_t length = wcslen(L"Aucun audio");
	wcscpy_s(summaryRecP->audioSummary, length + 1, L"Aucun audio");

    PrParam width;
    PrParam height;
    PrParam PARNum;
    PrParam PARDen;
    PrParam frameRate;

    // -- get parameters
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoWidth, &width);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoHeight, &height);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoFrameRate, &frameRate);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_PixelAspectNumerator, &PARNum);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_PixelAspectDenominator, &PARDen);

    // Get the timebase to compute the framerate
    PrTime ticksPerSecond;
    timeSuite->GetTicksPerSecond(&ticksPerSecond);

    // --- Video
    wchar_t summaryVideo[256];
    swprintf(summaryVideo, 256, L"%ix%i (%.4f), %.2f fps", width.mInt32, height.mInt32, 
        static_cast<float>(PARNum.mInt32) / static_cast<float>(PARDen.mInt32),
        static_cast<float>(ticksPerSecond) / static_cast<float>(frameRate.mInt64));
    length = wcslen(summaryVideo);
	wcscpy_s(summaryRecP->videoSummary, length + 1, summaryVideo);

    // --- Bitrate, just calculate the uncompressed raw rate
    wchar_t summaryBitrate[256];
    swprintf(summaryBitrate, 256, L"%.3f Mb/s", width.mInt32 * height.mInt32 * 4 * 8 / (1000000.0));
    length = wcslen(summaryBitrate);
	wcscpy_s(summaryRecP->bitrateSummary, length + 1, summaryBitrate);
}

bool ExporterSettings::exportToCis(exDoExportRec *exportInfoP)
{
    // -- retrieve filepath
    csSDK_int32 bufferlength;
    prUTF16Char tempFile[256];
    exportFileSuite->GetPlatformPath(exportInfoP->fileObject, &bufferlength, tempFile);
    std::wstring_convert<std::codecvt_utf8_utf16<prUTF16Char>, prUTF16Char> convert;
    filePath = convert.to_bytes(tempFile);

    exParamValues versionParam;
    exportParamSuite->GetParamValue(pluginId, 0, "VERSION_ID", &versionParam);
    int version = versionParam.value.intValue;

    if (version < 1 || version > 2) {
        return false;
    }

    // -- we get basic rendering parameters
    PrParam width;
    PrParam height;
    PrParam PARNum;
    PrParam PARDen;
    PrParam frameRate;
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoWidth, &width);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoHeight, &height);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_VideoFrameRate, &frameRate);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_PixelAspectNumerator, &PARNum);
    exportInfoSuite->GetExportSourceInfo(pluginId, kExportInfo_PixelAspectDenominator, &PARDen);
    widthF = width.mInt32;
    heightF = height.mInt32;
    frameRateF = frameRate.mInt64;

    // -- create videoRenderer instance
	sequenceRenderSuite->MakeVideoRenderer(pluginId, &videoRenderId, frameRate.mInt64);

    // -- define async callback after rendering
    PrSDKSequenceAsyncRenderCompletionProc func = reinterpret_cast<PrSDKSequenceAsyncRenderCompletionProc>(&treatRendering);
    sequenceRenderSuite->SetAsyncRenderCompletionProc(videoRenderId, func, this);

    // -- we set progress to 0
    exportProgressSuite->UpdateProgressPercent(pluginId, 0);
    totalDuration = exportInfoP->endTime - exportInfoP->startTime;
    startTimeF = exportInfoP->startTime;
    endTimeF = exportInfoP->startTime;

    // -- Creation of cis file
    cis = new CisWriter();
    cis->setImageFormat(widthF, heightF, 32);
    cis->setNbImages(totalDuration / frameRateF);
    cis->startSave(filePath);

    // -- execute rendering
    for (PrTime videoTime = exportInfoP->startTime; videoTime <= (exportInfoP->endTime - frameRate.mInt64); 
        videoTime += frameRate.mInt64) {

        // -- define render parameters
        SequenceRender_ParamsRec renderParams;
        PrPixelFormat pixelFormats[] = {PrPixelFormat_BGRA_4444_8u, PrPixelFormat_BGRA_4444_8u};
        renderParams.inRequestedPixelFormatArray = pixelFormats;
        renderParams.inRequestedPixelFormatArrayCount = 1;
        renderParams.inWidth = width.mInt32;
        renderParams.inHeight = height.mInt32;
        renderParams.inPixelAspectRatioNumerator = PARNum.mInt32;
        renderParams.inPixelAspectRatioDenominator = PARDen.mInt32;
        renderParams.inRenderQuality = kPrRenderQuality_High;
        renderParams.inFieldType = prFieldsNone;
        renderParams.inDeinterlace = kPrFalse;
        renderParams.inDeinterlaceQuality = kPrRenderQuality_High;
        renderParams.inCompositeOnBlack = kPrFalse;

        csSDK_uint32 value;
        sequenceRenderSuite->QueueAsyncVideoFrameRender(videoRenderId, videoTime, &value, &renderParams, kRenderCacheType_RenderedFrame, nullptr);
    }

    prSuiteError error = exportReturn_ErrNone;

    while (!cis->tryLockMutexSaving()) {}
    while (!cis->isFinished() && error != exportReturn_Abort && error != exportReturn_Done) {
        cis->trySave();

        // -- check progress
        float progress = cis->getProgress();
        error = exportProgressSuite->UpdateProgressPercent(pluginId, progress);
    }
    cis->unlockMutexSaving();

    // -- write end of cis file
    std::string versionString = "";
    if (version == 1) {versionString = "CIS-V001";}
    else if (version == 2) {versionString = "CIS-V002";}
    //else if (version == 3) {versionString = "CIS-V003";}

    cis->endSave(versionString);

    // -- release renderer
    sequenceRenderSuite->ReleaseVideoRenderer(pluginId, videoRenderId);

    return true;
}

void ExporterSettings::handleRenderingFrame(PrTime inTime, PPixHand inRenderedFrame, SequenceRender_GetFrameReturnRec *inGetFrameReturn)
{
    csSDK_int32 rowbytes = 0;
    char* frameBufferP = nullptr;
    

    // -- get buffer of pixels
    ppixSuite->GetPixels(inGetFrameReturn->outFrame, PrPPixBufferAccess_ReadOnly, &frameBufferP);
	ppixSuite->GetRowBytes(inGetFrameReturn->outFrame, &rowbytes);

    long frameNb = (((float)(inTime - startTimeF)) / ((float)frameRateF)) + 0.5;
    std::thread th(renderAndTrySaveFrame, this, frameNb, rowbytes, frameBufferP);
    th.join();
}

void ExporterSettings::removeRowPadding(char* srcFrame, char* dstFrame, csSDK_int32 rowBytes, csSDK_int32 pixelSize, csSDK_int32 widthL, csSDK_int32 heightL)
{
    csSDK_int32 widthBytes = widthL * pixelSize;
	if (widthBytes < rowBytes) {
		for (csSDK_int32 hL = 0; hL < heightL; ++hL) {
            memcpy(&dstFrame[hL * widthBytes], &srcFrame[(heightL-1-hL) * rowBytes], widthBytes);
		}
	}
}

void ExporterSettings::reverseImage(char* srcFrame, char* dstFrame, csSDK_int32 rowBytes, csSDK_int32 pixelSize, csSDK_int32 widthL, csSDK_int32 heightL)
{
    csSDK_int32 widthBytes = widthL * pixelSize;
    for (csSDK_int32 hL = 0; hL < heightL; ++hL) {
        memcpy(&dstFrame[hL * widthBytes], &srcFrame[(heightL-1-hL) * rowBytes], widthBytes);
    }
}

void treatRendering(csSDK_uint32 inVideoRenderIDm, void* inCallbackRef, 
        PrTime inTime, PPixHand inRenderedFrame, SequenceRender_GetFrameReturnRec *inGetFrameReturn)
{
    ExporterSettings* settings = reinterpret_cast<ExporterSettings*>(inCallbackRef);
    settings->handleRenderingFrame(inTime, inRenderedFrame, inGetFrameReturn);
}