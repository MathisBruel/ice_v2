#include "MovingControler.h"

PF_Err About(PF_InData *in_data, PF_OutData *out_data)
{
    PF_SPRINTF(out_data->return_msg, "%s  V%d.%d\r%s", "Moving Controler", 2, 0, "Made by Arthur Loosveldt");
    return PF_Err_NONE;
}
PF_Err GlobalSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    PF_Err err = PF_Err_NONE;

    out_data->my_version = PF_VERSION(2, 0, 0, 0, 1);
    out_data->out_flags = PF_OutFlag_DISPLAY_ERROR_MESSAGE | PF_OutFlag_CUSTOM_UI | PF_OutFlag_NON_PARAM_VARY | PF_OutFlag_SEQUENCE_DATA_NEEDS_FLATTENING; 
    out_data->out_flags2 = PF_OutFlag2_I_USE_TIMECODE;

    if ((out_data->global_data = PF_NEW_HANDLE(sizeof(PtrPresets)))) {
        PtrPresets* handle = (PtrPresets*)(*out_data->global_data);
        handle->ptr = new PresetLoader("C:/ICE-EFFECT");
        handle->ptr->loadPresets();
        
        std::string loadingError = handle->ptr->getLoadingError();
        if (!loadingError.empty()) {
            std::string outMsg = "Error at loading : \n" + loadingError;
            PF_STRCPY(out_data->return_msg, outMsg.c_str());
            out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        }
    }
    else {
        PF_STRCPY(out_data->return_msg, "Error create handle on sequence data !");
        out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }
    return err;
}
PF_Err GlobalSetdown(PF_InData *in_data, PF_OutData *out_data)
{
    PtrPresets* handle = (PtrPresets*)(*out_data->global_data);
    delete handle->ptr;
    PF_DISPOSE_HANDLE(in_data->global_data);
    return PF_Err_NONE;
}

PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    // -- create params
    PtrPresets* handle = (PtrPresets*)(*out_data->global_data);
    MovingHandler::createParams(in_data, handle->ptr->getPositionPresetsString(), handle->ptr->getColorPresetsString(), handle->ptr->getNbPresetPos(), handle->ptr->getNbPresetColor());
    return PF_Err_NONE;
}

PF_Err SequenceSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    PF_Err err = PF_Err_NONE;

    if (!in_data->sequence_data) {
        if ((out_data->sequence_data = PF_NEW_HANDLE(sizeof(PtrMovingHandler)))) {
            PtrMovingHandler* handle = (PtrMovingHandler*)(*out_data->sequence_data);
            handle->ptr = new MovingHandler();
            handle->flatten = false;
        }
        else {
            PF_STRCPY(out_data->return_msg, "Error create handle on sequence data !");
            out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
            err = PF_Err_INTERNAL_STRUCT_DAMAGED;
        }
    }

    return err;
}
PF_Err SequenceFlatten(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // -- get datas
    if (in_data->sequence_data) {
        PtrMovingHandler* handleUnflat = reinterpret_cast<PtrMovingHandler*>(*(in_data->sequence_data));

        if (!handleUnflat->flatten && handleUnflat->ptr->isSaveNeeded()) {
            PF_Handle flatSeq = suites.HandleSuite1()->host_new_handle(sizeof(FlatMovingHandler));
            if (flatSeq) {
                FlatMovingHandler *handleFlat = reinterpret_cast<FlatMovingHandler*>(suites.HandleSuite1()->host_lock_handle(flatSeq));
                AEFX_CLR_STRUCT(*handleFlat);
                handleFlat->flatten = true;
                handleUnflat->ptr->flat(handleFlat->flatData);
                delete handleUnflat->ptr;
                out_data->sequence_data = flatSeq;
                suites.HandleSuite1()->host_unlock_handle(flatSeq);
            }
        }
    }
    else {
        PF_STRCPY(out_data->return_msg, "Error : sequence data is not set !");
        out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }
    return err;
}
PF_Err SequenceResetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    PF_Err err = PF_Err_NONE;
	AEGP_SuiteHandler suites(in_data->pica_basicP);

    // -- get datas
    if (in_data->sequence_data) {

        FlatMovingHandler* handleFlat = reinterpret_cast<FlatMovingHandler*>(*(in_data->sequence_data));

        if (handleFlat->flatten) {
            PF_Handle unflatSeq = suites.HandleSuite1()->host_new_handle(sizeof(PtrMovingHandler));
            if (unflatSeq) {
                PtrMovingHandler *handleUnflat = reinterpret_cast<PtrMovingHandler*>(suites.HandleSuite1()->host_lock_handle(unflatSeq));
                AEFX_CLR_STRUCT(*handleUnflat);
                handleUnflat->ptr = new MovingHandler();
                handleUnflat->ptr->unflat(handleFlat->flatData);
                handleUnflat->flatten = false;
                // -- set size of sequence
                if (in_data->total_time != 1036800) {
                    long totalTime = in_data->total_time;
                    if (totalTime >= 86400) {totalTime -= 86400;}
                    handleUnflat->ptr->setNbFrames(totalTime);

                    // -- save into file for effect the size (to be reused when update is done)
                    if (handleUnflat->ptr->isSaveNeeded()) {
                        SizeSeqHandler seqHandler;
                        seqHandler.addId(handleUnflat->ptr->getUuid(), totalTime);
                    }
                }
                out_data->sequence_data = unflatSeq;
                suites.HandleSuite1()->host_unlock_handle(unflatSeq);
            }
        }
        else {
            PtrMovingHandler* handleUnflat = reinterpret_cast<PtrMovingHandler*>(*(in_data->sequence_data));
            if (in_data->total_time != 1036800) {
                long totalTime = in_data->total_time;
                if (totalTime >= 86400) {totalTime -= 86400;}
                handleUnflat->ptr->setNbFrames(totalTime);

                // -- save into file for effect the size (to be reused when update is done)
                if (handleUnflat->ptr->isSaveNeeded()) {
                    SizeSeqHandler seqHandler;
                    seqHandler.addId(handleUnflat->ptr->getUuid(), totalTime);
                }
            }
        }
    }
    else {
        PF_STRCPY(out_data->return_msg, "Error : sequence data is not set !");
        out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    return err;
}
PF_Err SequenceSetdown(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    SizeSeqHandler seqHandler;
    if (in_data->sequence_data) {
        FlatMovingHandler* handleFlat = reinterpret_cast<FlatMovingHandler*>(*(in_data->sequence_data));
        if (handleFlat->flatten) {
            MovingHandler* handler = new MovingHandler();
            handler->unflat(handleFlat->flatData);
            seqHandler.removeId(handler->getUuid());
            delete handler;
        }
        else {
            PtrMovingHandler* handleUnflat = reinterpret_cast<PtrMovingHandler*>(*(in_data->sequence_data));
            seqHandler.removeId(handleUnflat->ptr->getUuid());
            delete handleUnflat->ptr;
        }
    }

    AEGP_SuiteHandler suites(in_data->pica_basicP);
    suites.HandleSuite1()->host_dispose_handle(in_data->sequence_data);
    return PF_Err_NONE;
}

PF_Err Render(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
    // -- get datas
    PtrMovingHandler* handle = nullptr;
    if (in_data->sequence_data) {
        handle = (PtrMovingHandler*)*(out_data->sequence_data);
    }

    if (handle != nullptr && !handle->flatten && handle->ptr != nullptr) {
        MovingHandler* handler = handle->ptr;

        // -- handler rendering base
        handler->createCanvas(in_data, output);

        if (handler->isVisible()) {
            // -- create positions
            handler->createPositionLine(in_data, output, in_data->current_time);
            handler->createPositionPoints(in_data, output, in_data->current_time);
            // -- render color
            handler->createColorsRect(in_data, output, in_data->current_time);
        }
        // -- final rendering
        handler->createFinalRender(in_data, output, in_data->current_time);
    }

    return PF_Err_NONE;
}
PF_Err UserChangedParam(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], const PF_UserChangedParamExtra *which_hitP)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    PtrMovingHandler* handle = nullptr;
    if (in_data->sequence_data) {
        handle = (PtrMovingHandler*)*(out_data->sequence_data);
    }
    else {
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    if (handle != nullptr && handle->ptr != nullptr) {
        MovingHandler* handler = handle->ptr;
        int index = which_hitP->param_index;

        // -- set current frame : remove 1 hour (I dunno why ^-^)
        long frameImpacted = handler->getLastRendererFrame();
        long currentFrame = in_data->current_time - 86400;

        // -- moving head type
        if (index == ID_CHANGETYPE) {
            int type = params[ID_CHANGETYPE]->u.pd.value;
            handler->changetype((MovingHeadType)type);
        }
        // -- update parameters
        else if (index == ID_UPDATE) {
            
            // -- reset nbFrames from file
            SizeSeqHandler seqHandler;
            long sizeSequence = seqHandler.getSizeForId(handler->getUuid());
            if (sizeSequence != -1) handler->setNbFrames(sizeSequence);

		    handler->setLastRenderFrame(currentFrame);
            bool isPosSet = handler->isPositionSetAtTime(currentFrame);
            bool isColorSet = handler->isColorSetAtTime(currentFrame);
            Position* positionParam = handler->getPositionAtTime(currentFrame);
            Color* colorParam = handler->getColorAtTime(currentFrame);
            Interpolation* posInter = handler->getPosInterpolationAtTime(currentFrame);
            Interpolation* colorInter = handler->getColorInterpolationAtTime(currentFrame);

            // -- change appearence positions
            params[ID_ACTIVEPOS]->u.bd.value = (isPosSet) ? 1 : 0;
            params[ID_ACTIVEPOS]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            params[ID_POSITION]->ui_flags = (isPosSet) ? 0 : PF_PUI_DISABLED;
			suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_POSITION, params[ID_POSITION]);

            // -- change appearence colors
            params[ID_ACTIVECOLOR]->u.bd.value = (isColorSet) ? 1 : 0;
            params[ID_ACTIVECOLOR]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            params[ID_COLOR]->ui_flags = (isColorSet) ? 0 : PF_PUI_DISABLED;
            params[ID_COLOR_INTENSITY]->ui_flags = (isColorSet) ? 0 : PF_PUI_DISABLED;
			suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_COLOR, params[ID_COLOR]);
            suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_COLOR_INTENSITY, params[ID_COLOR_INTENSITY]);

            if (positionParam != nullptr) {
                params[ID_POSITION]->u.td.x_value = (long)(positionParam->ratioX * 100.0*65536);
                params[ID_POSITION]->u.td.y_value = (long)(positionParam->ratioY * 100.0*65536);
                params[ID_POSITION]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            }
            else {
                params[ID_POSITION]->u.td.x_value = (long)(0);
                params[ID_POSITION]->u.td.y_value = (long)(0);
                params[ID_POSITION]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            }

            if (posInter != nullptr) {
                params[ID_INTERPOS_TYPE]->u.pd.value = posInter->type;
                params[ID_INTERPOS_TYPE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_INTERPOS_EXTRA]->u.fs_d.value = posInter->extra;
                params[ID_INTERPOS_EXTRA]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_INTERPOS_EXTRA2]->u.fs_d.value = posInter->extra2;
                params[ID_INTERPOS_EXTRA2]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;

                if (posInter->type < 2) {
                    params[ID_INTERPOS_EXTRA]->ui_flags = PF_PUI_DISABLED;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA, params[ID_INTERPOS_EXTRA]);
                    params[ID_INTERPOS_EXTRA2]->ui_flags = PF_PUI_DISABLED;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA2, params[ID_INTERPOS_EXTRA2]);
                }
                else if (posInter->type == 2) {
                    params[ID_INTERPOS_EXTRA]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA, params[ID_INTERPOS_EXTRA]);
                    params[ID_INTERPOS_EXTRA2]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA2, params[ID_INTERPOS_EXTRA2]);
                }
                else if (posInter->type == 3) {
                    params[ID_INTERPOS_EXTRA]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA, params[ID_INTERPOS_EXTRA]);
                    params[ID_INTERPOS_EXTRA2]->ui_flags = PF_PUI_DISABLED;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA2, params[ID_INTERPOS_EXTRA2]);
                }
            }
            else {
                params[ID_INTERPOS_TYPE]->u.pd.value = 0;
                params[ID_INTERPOS_TYPE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_INTERPOS_EXTRA]->u.fs_d.value = 0.0;
                params[ID_INTERPOS_EXTRA]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_INTERPOS_EXTRA]->ui_flags = PF_PUI_DISABLED;
                suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA, params[ID_INTERPOS_EXTRA]);
                params[ID_INTERPOS_EXTRA2]->u.fs_d.value = 0.0;
                params[ID_INTERPOS_EXTRA2]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_INTERPOS_EXTRA2]->ui_flags = PF_PUI_DISABLED;
                suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA2, params[ID_INTERPOS_EXTRA2]);
            }

            if (colorParam != nullptr) {
                params[ID_COLOR]->u.cd.value.red = colorParam->red;
                params[ID_COLOR]->u.cd.value.green = colorParam->green;
                params[ID_COLOR]->u.cd.value.blue = colorParam->blue;
                params[ID_COLOR]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_COLOR_INTENSITY]->u.fs_d.value = colorParam->alpha;
                params[ID_COLOR_INTENSITY]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            }
            else {
                params[ID_COLOR]->u.cd.value.red = 0;
                params[ID_COLOR]->u.cd.value.green = 0;
                params[ID_COLOR]->u.cd.value.blue = 0;
                params[ID_COLOR]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_COLOR_INTENSITY]->u.fs_d.value = 255;
                params[ID_COLOR_INTENSITY]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            }

            if (colorInter != nullptr) {
                params[ID_INTERCOLOR_TYPE]->u.pd.value = colorInter->type;
                params[ID_INTERCOLOR_TYPE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_INTERCOLOR_EXTRA]->u.fs_d.value = colorInter->extra;
                params[ID_INTERCOLOR_EXTRA]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;

                if (colorInter->type < 2) {
                    params[ID_INTERCOLOR_EXTRA]->ui_flags = PF_PUI_DISABLED;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERCOLOR_EXTRA, params[ID_INTERCOLOR_EXTRA]);
                }
                else if (colorInter->type == 2) {
                    params[ID_INTERCOLOR_EXTRA]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERCOLOR_EXTRA, params[ID_INTERCOLOR_EXTRA]);
                }
                else if (colorInter->type == 3) {
                    params[ID_INTERCOLOR_EXTRA]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERCOLOR_EXTRA, params[ID_INTERCOLOR_EXTRA]);
                }
            }
            else {
                params[ID_INTERCOLOR_TYPE]->u.pd.value = 0;
                params[ID_INTERCOLOR_TYPE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_INTERCOLOR_EXTRA]->u.fs_d.value = 50.0;
                params[ID_INTERCOLOR_EXTRA]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
                params[ID_INTERCOLOR_EXTRA]->ui_flags = PF_PUI_DISABLED;
                suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERCOLOR_EXTRA, params[ID_INTERCOLOR_EXTRA]);
            }
        
            // -- timeline parameters 
            params[ID_SCALE_POS]->u.fs_d.value = handler->getResizeFactor();
            params[ID_SCALE_POS]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            params[ID_TRANSLATE_TIMELINE]->u.fs_d.value = handler->getTimelineTranslation();
            params[ID_TRANSLATE_TIMELINE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            params[ID_MODE_LOOP]->u.pd.value = handler->isModeLoopReversed() ? 2 : 1;
            params[ID_MODE_LOOP]->uu.change_flags = PF_ChangeFlag_CHANGED_VALUE;

            // -- transformation parameters
            params[ID_SCALE_POS]->u.fs_d.value = handler->getScaleFactor();
            params[ID_SCALE_POS]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            params[ID_TRANSLATE_POS]->u.td.x_value = handler->getTranslationX()*100.0*65536.0;
            params[ID_TRANSLATE_POS]->u.td.y_value = handler->getTranslationY()*100.0*65536.0;
            params[ID_TRANSLATE_POS]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            params[ID_ROTATION_ANGLE]->u.ad.value = handler->getRotationAngle()*65536.0;
            params[ID_ROTATION_ANGLE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            params[ID_ROTATION_CENTER]->u.td.x_value = handler->getCenterRotationX()*100.0*65536.0;
            params[ID_ROTATION_CENTER]->u.td.y_value = handler->getCenterRotationY()*100.0*65536.0;
            params[ID_ROTATION_CENTER]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;

            handler->setSave();
        }
        // -- visible
        else if (index == ID_VISIBLE) {
            handler->setVisible(params[ID_VISIBLE]->u.bd.value == 1);
        }

        // -- activate/deactivate position
        else if (index == ID_ACTIVEPOS) {
            
            bool isActive = (bool)params[index]->u.bd.value;
            // -- frame is correct
            if (frameImpacted == currentFrame) {
                // -- activate positions
                params[ID_POSITION]->ui_flags = (isActive) ? 0 : PF_PUI_DISABLED;
                suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_POSITION, params[ID_POSITION]);

                // -- create position & interpolation
                if (isActive) {
                    handler->createPosition(params[ID_POSITION]->u.td.x_value/65536.0, params[ID_POSITION]->u.td.y_value/65536.0);
                }
                else {
                    handler->removePosition();
                }
            }

            // -- need to update before modification
            else {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame is not correct. Changes no taken into account, please update !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
        }
        // -- change position
        else if (index == ID_POSITION) {
            // -- frame is correct
            if (frameImpacted == currentFrame) {

                double valueX = params[ID_POSITION]->u.td.x_value/65536.0;
                double valueY = params[ID_POSITION]->u.td.y_value/65536.0;

                valueX = std::fmax(0.0, std::fmin(valueX, 100.0));
                valueY = std::fmax(0.0, std::fmin(valueY, 100.0));
                params[ID_POSITION]->u.td.x_value = valueX*65536;
                params[ID_POSITION]->u.td.y_value = valueY*65536;
                params[ID_POSITION]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;

                // -- update position & interpolation
                handler->createPosition(valueX, valueY);

            }
            // -- need to update before modification
            else {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame is not correct. Changes no taken into account, please update !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
        }
        // -- change type interpolation
        else if (index == ID_INTERPOS_TYPE) {
            // -- frame is correct
            if (frameImpacted == currentFrame) {

                int type = params[ID_INTERPOS_TYPE]->u.pd.value;
                handler->changePosInterpolate(type, params[ID_INTERPOS_EXTRA]->u.fs_d.value, params[ID_INTERPOS_EXTRA2]->u.fs_d.value);

                // -- LINEAR
                if (type < 2) {
                    params[ID_INTERPOS_EXTRA]->ui_flags = PF_PUI_DISABLED;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA, params[ID_INTERPOS_EXTRA]);
                    params[ID_INTERPOS_EXTRA2]->ui_flags = PF_PUI_DISABLED;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA2, params[ID_INTERPOS_EXTRA2]);
                }
                // -- CURVE
                else if (type == 2) {
                    params[ID_INTERPOS_EXTRA]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA, params[ID_INTERPOS_EXTRA]);
                    params[ID_INTERPOS_EXTRA2]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA2, params[ID_INTERPOS_EXTRA2]);
                }
                // -- ARC
                else if (type == 3) {
                    params[ID_INTERPOS_EXTRA]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA, params[ID_INTERPOS_EXTRA]);
                    params[ID_INTERPOS_EXTRA2]->ui_flags = PF_PUI_DISABLED;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERPOS_EXTRA2, params[ID_INTERPOS_EXTRA2]);
                }
            }
            else {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame is not correct. Changes no taken into account, please update !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
        }
        // -- change extra or extra2 interpolation
        else if (index == ID_INTERPOS_EXTRA || index == ID_INTERPOS_EXTRA2) {
            // -- frame is correct
            if (frameImpacted == currentFrame) {
                handler->changePosInterpolate(params[ID_INTERPOS_TYPE]->u.pd.value, params[ID_INTERPOS_EXTRA]->u.fs_d.value, params[ID_INTERPOS_EXTRA2]->u.fs_d.value);
            }
            else {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame is not correct. Changes no taken into account, please update !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
        }

        // -- activate/deactivate color
        else if (index == ID_ACTIVECOLOR) {
            bool isActive = (bool)params[index]->u.bd.value;

            // -- frame is correct
            if (frameImpacted == currentFrame) {
                // -- activate positions
                params[ID_COLOR]->ui_flags = (isActive) ? 0 : PF_PUI_DISABLED;
                suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_COLOR, params[ID_COLOR]);
                params[ID_COLOR_INTENSITY]->ui_flags = (isActive) ? 0 : PF_PUI_DISABLED;
                suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_COLOR_INTENSITY, params[ID_COLOR_INTENSITY]);

                // -- create color & interpolation
                if (isActive) {
                    handler->createColor(params[ID_COLOR]->u.cd.value.red, params[ID_COLOR]->u.cd.value.green, params[ID_COLOR]->u.cd.value.blue, params[ID_COLOR_INTENSITY]->u.fs_d.value);
                }
                else {
                    handler->removeColor();
                }
            }
            // -- need to update before modification
            else {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame is not correct. Changes no taken into account, please update !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
        }
        // -- change color or intensity
        else if (index == ID_COLOR || index == ID_COLOR_INTENSITY) {

            // -- frame is correct
            if (frameImpacted == currentFrame) {
                // -- update color & interpolation
                handler->createColor(params[ID_COLOR]->u.cd.value.red, params[ID_COLOR]->u.cd.value.green, params[ID_COLOR]->u.cd.value.blue, params[ID_COLOR_INTENSITY]->u.fs_d.value);
            }
            // -- need to update before modification
            else {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame is not correct. Changes no taken into account, please update !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
        }
        // -- change type interpolation
        else if (index == ID_INTERCOLOR_TYPE) {
            // -- frame is correct
            if (frameImpacted == currentFrame) {
                int type = params[ID_INTERCOLOR_TYPE]->u.pd.value;
                handler->changeColorInterpolate(type, params[ID_INTERCOLOR_EXTRA]->u.fs_d.value);

                // -- LINEAR
                if (type < 2) {
                    params[ID_INTERCOLOR_EXTRA]->ui_flags = PF_PUI_DISABLED;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERCOLOR_EXTRA, params[ID_INTERCOLOR_EXTRA]);
                }
                // -- SLOW or FAST
                else {
                    params[ID_INTERCOLOR_EXTRA]->ui_flags = 0;
                    suites.ParamUtilsSuite3()->PF_UpdateParamUI(in_data->effect_ref, ID_INTERCOLOR_EXTRA, params[ID_INTERCOLOR_EXTRA]);
                }
            }
            else {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame is not correct. Changes no taken into account, please update !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
        }
        // -- change extra interpolation
        else if (index == ID_INTERCOLOR_EXTRA) {
            // -- frame is correct
            if (frameImpacted == currentFrame) {
                handler->changeColorInterpolate(params[ID_INTERCOLOR_TYPE]->u.pd.value, params[ID_INTERCOLOR_EXTRA]->u.fs_d.value);
            }
            else {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame is not correct. Changes no taken into account, please update !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
        }

        // -- resize value
        else if (index == ID_RESIZE_TIMELINE) {
            handler->setResize(params[ID_RESIZE_TIMELINE]->u.fs_d.value);
        }
        else if (index == ID_APPLY_RESIZE_TIMELINE) {
            handler->applyTimelineResize();
            params[ID_RESIZE_TIMELINE]->u.fs_d.value = 1.0;
            params[ID_RESIZE_TIMELINE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
        }
        // -- translate timeline
        else if (index == ID_TRANSLATE_TIMELINE) {
            handler->setTimelineTranslation(params[ID_TRANSLATE_TIMELINE]->u.fs_d.value);
        }
        else if (index == ID_APPLY_TRANSLATE_TIMELINE) {
            handler->applyTimelineTranslation();
            params[ID_TRANSLATE_TIMELINE]->u.fs_d.value = 0.0;
            params[ID_TRANSLATE_TIMELINE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
        }
        // -- reverse time positions
        else if (index == ID_REVERSE_TIMELINE) {
            handler->reverseTimeline();
        }
        // -- mode loop
        else if (index == ID_MODE_LOOP) {
            bool reverse = (params[ID_MODE_LOOP]->u.pd.value == 2);
            handler->setLoopMode(reverse);
        }
        // -- loop
        else if (index == ID_APPLY_LOOP) {
            handler->loop();
        }

        // -- flip horizontally
        else if (index == ID_FLIP_POS_H) {
            handler->flipH();
        }
        // -- flip vertically
        else if (index == ID_FLIP_POS_V) {
            handler->flipV();
        }
        // -- scale factor
        else if (index == ID_SCALE_POS) {
            handler->changeScaleFactor(params[ID_SCALE_POS]->u.fs_d.value);
        }
        // -- scale apply
        else if (index == ID_APPLY_SCALE_POS) {
            handler->applyScaling();
            params[ID_SCALE_POS]->u.fs_d.value = 1.0;
            params[ID_SCALE_POS]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
        }
        // -- translate vector
        else if (index == ID_TRANSLATE_POS) {

            double valueX = params[ID_TRANSLATE_POS]->u.td.x_value / 65536.0;
            double valueY = params[ID_TRANSLATE_POS]->u.td.y_value / 65536.0;

            // -- check in/out X
            valueX = std::fmax(-100, std::fmin(valueX, 100));
            valueY = std::fmax(-100, std::fmin(valueY, 100));
            params[ID_TRANSLATE_POS]->u.td.x_value = valueX*65536;
            params[ID_TRANSLATE_POS]->u.td.y_value = valueY*65536;
            params[ID_TRANSLATE_POS]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            handler->changeTranslationVector(valueX / 100.0, valueY / 100.0);
        }
        // -- translate apply
        else if (index == ID_APPLY_TRANSLATE_POS) {
            handler->applyTranslation();
            params[ID_TRANSLATE_POS]->u.td.x_value = 0.0;
            params[ID_TRANSLATE_POS]->u.td.y_value = 0.0;
            params[ID_TRANSLATE_POS]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
        }
        // -- rotate angle
        else if (index == ID_ROTATION_ANGLE) {
            handler->changeRotationAngle(params[ID_ROTATION_ANGLE]->u.ad.value/65536.0);
        }
        // -- rotate center
        else if (index == ID_ROTATION_CENTER) {
            double valueX = params[ID_ROTATION_CENTER]->u.td.x_value / 65536.0;
            double valueY = params[ID_ROTATION_CENTER]->u.td.y_value / 65536.0;

            valueX = std::fmax(0.0, std::fmin(valueX, 100.0));
            valueY = std::fmax(0.0, std::fmin(valueY, 100.0));
            params[ID_ROTATION_CENTER]->u.td.x_value = valueX*65536;
            params[ID_ROTATION_CENTER]->u.td.y_value = valueY*65536;
            params[ID_ROTATION_CENTER]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            handler->changeCenterRotation(valueX / 100.0, valueY / 100.0);
        }
        // -- rotate apply
        else if (index == ID_APPLY_ROTATION) {
            handler->applyRotation();
            params[ID_ROTATION_ANGLE]->u.ad.value = 0.0;
            params[ID_ROTATION_ANGLE]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
            params[ID_ROTATION_CENTER]->u.td.x_value = 50.0*65536.0;
            params[ID_ROTATION_CENTER]->u.td.y_value = 50.0*65536.0;
            params[ID_ROTATION_CENTER]->uu.change_flags |= PF_ChangeFlag_CHANGED_VALUE;
        }

        // -- load presets positions
        else if (index == ID_LOAD_PRESET_POS) {
            int index = params[ID_LIST_PRESET_POS]->u.pd.value;
            PtrPresets* handlePresets = (PtrPresets*)(*out_data->global_data);
            MovingHandler* ref = handlePresets->ptr->getHandlerPositionWithIndex(index);

            if (ref != nullptr) {
                if (ref->getNbFrames() > handler->getNbFrames()) {
                    PF_STRCPY(out_data->return_msg, "WARNING : Current frame number is smaller than nb of frames to load !");
                    out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                    err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
                }
                else {
                    handler->reloadPositions(ref);
                }
            }
        }
        // -- save presets positions
        else if (index == ID_SAVE_PRESET_POS) {
            PtrPresets* handlePresets = (PtrPresets*)(*out_data->global_data);
            std::string filename = handlePresets->ptr->savePresets(handler, false);
            std::string outMessage = "Saving preset : Position preset " + filename + " saved ! Rename it before reload premiere pro to use it !";
            PF_STRCPY(out_data->return_msg, outMessage.c_str());
            out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        }
        // -- load presets colors
        else if (index == ID_LOAD_PRESET_COLOR) {
            int index = params[ID_LIST_PRESET_COLOR]->u.pd.value;
            PtrPresets* handlePresets = (PtrPresets*)(*out_data->global_data);
            MovingHandler* ref = handlePresets->ptr->getHandlerColorWithIndex(index);

            if (ref->getNbFrames() > handler->getNbFrames()) {
                PF_STRCPY(out_data->return_msg, "WARNING : Current frame number is smaller than nb of frames to load !");
                out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
                err = PF_Err_UNRECOGNIZED_PARAM_TYPE;
            }
            else {
                handler->reloadColors(ref);
            }
        }
        // -- save presets colors
        else if (index == ID_SAVE_PRESET_COLOR) {
            PtrPresets* handlePresets = (PtrPresets*)(*out_data->global_data);
            std::string filename = handlePresets->ptr->savePresets(handler, true);
            std::string outMessage = "Saving preset : Color preset " + filename + " saved ! Rename it before reload premiere pro to use it !";
            PF_STRCPY(out_data->return_msg, outMessage.c_str());
            out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        }

        out_data->out_flags |= PF_OutFlag_FORCE_RERENDER;
    }

    return err;
}

DllExport PF_Err EntryPointFunc(PF_Cmd cmd, PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output, void *extra)
{
    PF_Err err = PF_Err_NONE;

    try {
        switch (cmd) {
        case PF_Cmd_ABOUT:
            err = About(in_data, out_data);
            break;

        case PF_Cmd_GLOBAL_SETUP:
            err = GlobalSetup(in_data, out_data, params, output);
            break;

        case PF_Cmd_PARAMS_SETUP:
            err = ParamsSetup(in_data, out_data, params, output);
            break;

        case PF_Cmd_GLOBAL_SETDOWN:
            err = GlobalSetdown(in_data, out_data);
            break;

        case PF_Cmd_SEQUENCE_SETUP:
            err = SequenceSetup(in_data, out_data, params, output);
            break;

        case PF_Cmd_SEQUENCE_RESETUP:
          err = SequenceResetup(in_data, out_data, params, output);
            break;

        case PF_Cmd_SEQUENCE_FLATTEN:
            err = SequenceFlatten(in_data, out_data, params, output);
            break;

        case PF_Cmd_SEQUENCE_SETDOWN:
            err = SequenceSetdown(in_data, out_data, params, output);
            break;

        case PF_Cmd_RENDER:
            err = Render(in_data, out_data, params, output);
            break;

        case PF_Cmd_USER_CHANGED_PARAM:
            err = UserChangedParam(in_data, out_data, params, reinterpret_cast<const PF_UserChangedParamExtra *>(extra));
            break;

        case PF_Cmd_UPDATE_PARAMS_UI:
            err = PF_Err_NONE;
            break;

        case PF_Cmd_EVENT:
            err = PF_Err_NONE;
            break;

        }
    }
    catch (PF_Err &thrown_err) {
        err = thrown_err;
    }
    return err;
}