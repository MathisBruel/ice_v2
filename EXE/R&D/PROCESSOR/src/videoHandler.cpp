#include "videoHandler.h"

VideoHandler::VideoHandler() 
{
    thread = nullptr;
    modeOpen = true;
    nbFrames = -1;
    nbImages = 0;
}
VideoHandler::~VideoHandler() 
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void VideoHandler::openVideo(std::string filename)
{
    finished = false;
    modeOpen = true;
    this->filename = filename;
    nbFrames = -1;
    nbImages = 0;
    thread = new Poco::Thread("SeamCarving");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGHEST);
    thread->start(*this);
}
void VideoHandler::writeVideo(int64_t nbFrames, AVCodecParameters* codecParams, AVPixelFormat pixelFormat, std::string filename)
{
    finished = false;
    modeOpen = false;
    nbImages = 0;
    this->codecParams = codecParams;
    this->nbFrames = nbFrames;
    this->filename = filename;
    this->pixelFormat = pixelFormat;
    imgs = new Image*[nbFrames];
    for (int i = 0; i < nbFrames; i++) {imgs[i] = nullptr;}
    thread = new Poco::Thread("SeamCarving");
    thread->setPriority(Poco::Thread::Priority::PRIO_HIGHEST);
    thread->start(*this);
}

bool VideoHandler::hasImageAtIndex(int64_t idx)
{
    if (idx < nbFrames) {
        return imgs[idx] != nullptr;
    }
    return false;
}
void VideoHandler::addImageAtIndex(int64_t idx, Image* img)
{
    imgs[idx] = img;
    nbImages++;
}
Image* VideoHandler::getImageAtIndex(int64_t idx)
{
    if (idx < nbFrames) {
        return imgs[idx];
    }
    return nullptr;
}
void VideoHandler::deleteImageAtIndex(int64_t idx)
{
    if (idx < nbFrames) {
        imgs[idx] = nullptr;
        nbImages--;
    }
}

void VideoHandler::run()
{
    if (modeOpen) {
        // -- allocate context for opening video file
        AVFormatContext* context = avformat_alloc_context();
        if (context == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't allocate format context !", __FILE__, __LINE__);
            return;
        }
        // -- actually open video file
        if (avformat_open_input(&context, filename.c_str(), nullptr, nullptr) != 0) {
            Poco::Logger::get("VideoHandler").error("Could not open file !", __FILE__, __LINE__);
            return;
        }

        AVStream* stream = nullptr;
        int video_stream_index = -1;

        // -- check streams to find image stream
        for (uint8_t i = 0; i < context->nb_streams; i++) {
            if(context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                codecParams = context->streams[i]->codecpar;
                video_stream_index = i;
                stream = context->streams[i];
                break;
            }
            else {
                // -- ignored : surely audio
            }
        }

        const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
        if (codec == nullptr) {
            Poco::Logger::get("VideoHandler").error("Could not find codec for stream !", __FILE__, __LINE__);
            return;
        }

        // -- codec context for actually extract decoding information from video
        AVCodecContext* codecContext = avcodec_alloc_context3(codec);
        if (codecContext == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't allocate codec context !", __FILE__, __LINE__);
            return;
        }
        if (avcodec_parameters_to_context(codecContext, codecParams) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't set codec context parameters !", __FILE__, __LINE__);
            return;
        }
        if (avcodec_open2(codecContext, codec, nullptr) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't open codec context !", __FILE__, __LINE__);
            return;
        }

        // -- decode structure init
        AVFrame* frame = av_frame_alloc();
        if (frame == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't allocate frame !", __FILE__, __LINE__);
            return;
        }
        AVPacket* packet = av_packet_alloc();
        if (packet == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't allocate packet !", __FILE__, __LINE__);
            return;
        }

        // -- read packets to get frames
        int64_t currentIdx = 0;
        SwsContext* recolorContext = nullptr;
        while (av_read_frame(context, packet) >= 0) {

            if (packet->stream_index != video_stream_index) {
                continue;
            }
            int response = avcodec_send_packet(codecContext, packet);
            if (response < 0) {
                Poco::Logger::get("VideoHandler").error("Error while pushing packet of context !", __FILE__, __LINE__);
                break;
            }
            response = avcodec_receive_frame(codecContext, frame);
            if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                // -- empty frame
                continue;
            }
            else if (response < 0) {
                Poco::Logger::get("VideoHandler").error("Error while decoding packet !", __FILE__, __LINE__);
                break;
            }

            if (recolorContext == nullptr) {
                nbFrames = context->streams[video_stream_index]->nb_frames;
                imgs = new Image*[nbFrames];
                for (int i = 0; i < nbFrames; i++) {imgs[i] = nullptr;}
                pixelFormat = codecContext->pix_fmt;
                recolorContext = sws_getContext(frame->width, frame->height, codecContext->pix_fmt,
                    frame->width, frame->height, AV_PIX_FMT_RGB24,
                    SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
            }

            unsigned char* data = new unsigned char[frame->width * frame->height * 3];
            unsigned char* dest[3] = { data, nullptr, nullptr };
            int dest_linesize[3] = { frame->width * 3, 0, 0 };
            sws_scale(recolorContext, frame->data, frame->linesize, 0, frame->height, dest, dest_linesize);

            Image* img = new Image(frame->width, frame->height, PNG_FORMAT_RGB, data);
            av_packet_unref(packet);
            addImageAtIndex(currentIdx, img);
            currentIdx++;

            // -- check we don't load too many frames at the time
            while (nbImages >= 100) {
                Timer::crossUsleep(10000);
            }
        }

        // -- close and free datas
        sws_freeContext(recolorContext);
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecContext);
        avformat_close_input(&context);
        avformat_free_context(context);
        finished = true;
    }
    else {

        // -- wait for first frame
        int64_t currentIdx = 0;
        while (!hasImageAtIndex(currentIdx)) {Timer::crossUsleep(10000);}
        Image* currentImg = getImageAtIndex(currentIdx);

        int width = currentImg->getWidth();
        int height = currentImg->getHeight();

        // -- open format output
        AVFormatContext* formatContext = nullptr;
        if (avformat_alloc_output_context2(&formatContext, nullptr, nullptr, filename.c_str()) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't create format context !", __FILE__, __LINE__);
            return;
        }

        // -- open format context
        const AVOutputFormat* formatOutput = formatContext->oformat;
        
        AVStream* stream = avformat_new_stream(formatContext, nullptr);
        if (stream == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't create stream !", __FILE__, __LINE__);
            return;
        }
        stream->id = formatContext->nb_streams-1;

        // -- find encoder 
        const AVCodec *codec = avcodec_find_encoder(codecParams->codec_id);
        if (codec == nullptr) {
            Poco::Logger::get("VideoHandler").error("Could not find codec for stream !", __FILE__, __LINE__);
            return;
        }

        // -- codec context for actually encode frames
        AVCodecContext* codecContext = avcodec_alloc_context3(codec);
        if (codecContext == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't allocate codec context !", __FILE__, __LINE__);
            return;
        }
        if (avcodec_parameters_to_context(codecContext, codecParams) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't set codec context parameters !", __FILE__, __LINE__);
            return;
        }

        if (codecParams->codec_id == AV_CODEC_ID_PRORES) {
            codecContext->profile = FF_PROFILE_PRORES_PROXY;
        }
        codecContext->width = width;
        codecContext->height = height;
        codecContext->pix_fmt = pixelFormat;
        codecContext->time_base = AVRational{1, 24};
        codecContext->framerate = AVRational{24, 1};
        
        stream->time_base = codecContext->time_base;
        stream->avg_frame_rate = codecContext->framerate;

        /* Some formats want stream headers to be separate. */
        if (formatOutput->flags & AVFMT_GLOBALHEADER) {
            codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

         // -- initialize packet to puish to video
        AVPacket* packet = av_packet_alloc();
        if (packet == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't allocate packet !", __FILE__, __LINE__);
            return;
        }

        // -- open codecContext
        if (avcodec_open2(codecContext, codec, nullptr) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't open codec context !", __FILE__, __LINE__);
            return;
        }

        // -- initialize frame
        AVFrame* frame = av_frame_alloc();
        if (frame == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't allocate frame !", __FILE__, __LINE__);
            return;
        }
        frame->format = codecContext->pix_fmt;
        frame->width = width; 
        frame->height = height;
        if (av_frame_get_buffer(frame, 0) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't allocate frame buffer !", __FILE__, __LINE__);
            return;
        }

        if (avcodec_parameters_from_context(stream->codecpar, codecContext) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't copy stream parameters !", __FILE__, __LINE__);
            return;
        }

        SwsContext* colorContext = sws_getContext(width, height, AV_PIX_FMT_RGB24, 
                                                width, height, codecContext->pix_fmt,
                                                SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
        if (colorContext == nullptr) {
            Poco::Logger::get("VideoHandler").error("Can't create color context !", __FILE__, __LINE__);
            return;
        }

        if (avio_open(&formatContext->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't open file !", __FILE__, __LINE__);
            return;
        }

        if (avformat_write_header(formatContext, nullptr) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't write header to output file !", __FILE__, __LINE__);
            return;
        }

        if (av_image_alloc(frame->data, frame->linesize, width, height, codecContext->pix_fmt, 32) < 0) {
            Poco::Logger::get("VideoHandler").error("Can't allocate frame", __FILE__, __LINE__);
            return;
        }

        while (currentIdx < nbFrames-1) {
            if (currentImg != nullptr) {

                /*if (av_frame_make_writable(frame) < 0) {
                    Poco::Logger::get("VideoHandler").error("Frame writable error", __FILE__, __LINE__);
                    break;
                }*/

                unsigned char* dest[3] = {currentImg->getData(), nullptr, nullptr};
                int dest_linesize[3] = {width * 3, 0, 0};
                sws_scale(colorContext, dest, dest_linesize, 0, height, frame->data, frame->linesize);
                frame->pts = currentIdx;
                writeFrame(formatContext, codecContext, stream, frame, packet);
                deleteImageAtIndex(currentIdx);
                currentIdx++;
                delete currentImg;
            }
            else {
                Timer::crossUsleep(10000);
            }
            currentImg = getImageAtIndex(currentIdx);
        }

        writeFrame(formatContext, codecContext, stream, nullptr, packet);
        av_write_trailer(formatContext);
        avio_closep(&formatContext->pb);
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecContext);
        avformat_free_context(formatContext);
        finished = true;
    }
}

int VideoHandler::writeFrame(AVFormatContext *format, AVCodecContext *codec, AVStream *stream, AVFrame *frame, AVPacket *packet)
{
    int ret;

    // send the frame to the encoder
    ret = avcodec_send_frame(codec, frame);
    if (ret < 0) {
        Poco::Logger::get("VideoHandler").debug("Error while sending frame to codec !", __FILE__, __LINE__);
        return -1;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(codec, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0) {
            Poco::Logger::get("VideoHandler").debug("Error while encoding frame !", __FILE__, __LINE__);
            return -1;
        }

        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(packet, codec->time_base, stream->time_base);
        packet->stream_index = stream->index;

        ret = av_interleaved_write_frame(format, packet);
        av_packet_unref(packet);
        /* pkt is now blank (av_interleaved_write_frame() takes ownership of
        * its contents and resets pkt), so that no unreferencing is necessary.
        * This would be different if one used av_write_frame(). */
        if (ret < 0) {
            Poco::Logger::get("VideoHandler").debug("Error while writing frame !", __FILE__, __LINE__);
            return -1;
        }
    }

    return ret == AVERROR_EOF ? 1 : 0;
}