#include "streamReceiver.h"

StreamReceiver::StreamReceiver(std::string address, uint16_t port)
{
    this->address = address;
    this->port = port;
    server = nullptr;
    thread = nullptr;
    stop = lutApplied = false;
    currentFrame = -1;
}

StreamReceiver::~StreamReceiver()
{
    stop = true;
    if (thread != nullptr)
	{
		thread->tryJoin(50);
		delete thread;
        thread = nullptr;
	}
    if (server != nullptr) {
        server->close();
        delete server;
        server = nullptr;
    }
}

bool StreamReceiver::startThread()
{
    Poco::Net::SocketAddress socketAddress(address, port);
    Poco::Logger::get("StreamReceiver").debug("Opening stream socket : " + address + ":" + std::to_string(port), __FILE__, __LINE__);

    try {
        server = new Poco::Net::ServerSocket(socketAddress);
    }
    catch (std::exception &e) {
        Poco::Logger::get("StreamReceiver").error("Error while opening stream socket !", __FILE__, __LINE__);
        Poco::Logger::get("StreamReceiver").error(e.what(), __FILE__, __LINE__);
        return false;
    }

    currentFrame = 0;
    stop = lutApplied = false;
    thread = new Poco::Thread("Stream");
    thread->setPriority(Poco::Thread::Priority::PRIO_NORMAL);
	thread->start(*this);
    return true;
}

void StreamReceiver::stopThread() 
{
    stop = true;
    if (thread != nullptr)
	{
		thread->tryJoin(50);
		delete thread;
        thread = nullptr;
	}
    if (server != nullptr) {
        server->close();
        delete server;
        server = nullptr;
    }
    while (!mutexImages.tryLock()) {Timer::crossUsleep(20);}
    while (images.size() > 0) {images.pop();}
    mutexImages.unlock();
    Poco::Logger::get("StreamReceiver").debug("Closing stream socket !", __FILE__, __LINE__);
}

std::shared_ptr<Image> StreamReceiver::getLastAddedImage()
{
    while (!mutexImages.tryLock()) {Timer::crossUsleep(20);}
    while (images.size() > 1)
    {
        images.pop();
    }
    std::shared_ptr<Image> img;
    if (images.size() == 1) {
        img = images.front();
    }
    mutexImages.unlock();
    return img;
}

void StreamReceiver::removeLastAddedImage()
{
    while (!mutexImages.tryLock()) {Timer::crossUsleep(20);}
    while (images.size() > 0)
    {
        images.pop();
    }
    mutexImages.unlock();
}

int StreamReceiver::getCurrentFrame() 
{
    int frame;
    while (!mutexImages.tryLock()) {Timer::crossUsleep(20);}
    frame = currentFrame;
    mutexImages.unlock();
    return frame;
}

void StreamReceiver::run()
{
    uint8_t buffer[720*576*5];
    Poco::Timespan span(0, 10000);
    Poco::Logger::get("StreamReceiver").debug("Waiting for connection", __FILE__, __LINE__);
    Poco::Net::StreamSocket stream = server->acceptConnection();
    Poco::Logger::get("StreamReceiver").debug("Stream connected", __FILE__, __LINE__);
    stream.setKeepAlive(true);

    while (!stop) 
    {
        if (stream.poll(span, Poco::Net::Socket::SELECT_READ)) {

            bool nextLoop = false;
            uint32_t magic = 0;

            // -- read first header
            uint64_t timestampStart = Poco::Timestamp().epochMicroseconds();
            while (stream.available() < 1 && !nextLoop) {
                Timer::crossUsleep(250);
                long deltaTime = Poco::Timestamp().epochMicroseconds() - timestampStart;
                if (deltaTime > 100000) {
                    nextLoop = true;
                }
            }

            if (nextLoop) {
                Poco::Logger::get("StreamReceiver").debug("First byte not received : disconnect and reconnect", __FILE__, __LINE__);
                stream.close();
                stream = server->acceptConnection();
                stream.setKeepAlive(true);
                continue;
            }

            int nbBytes = stream.receiveBytes(buffer, 1);
            if (buffer[0] == 0x4e || buffer[0] == 0xb1)
            {
                Poco::Logger::get("StreamReceiver").debug("First byte received", __FILE__, __LINE__);
                timestampStart = Poco::Timestamp().epochMicroseconds();
                while (stream.available() < 3 && !nextLoop) {
                    Timer::crossUsleep(250);
                    long deltaTime = Poco::Timestamp().epochMicroseconds() - timestampStart;
                    if (deltaTime > 100000) {
                        nextLoop = true;
                    }
                }
                if (!nextLoop) {
                    Poco::Logger::get("StreamReceiver").debug("Bytes of magic received", __FILE__, __LINE__);
                    nbBytes = stream.receiveBytes(&buffer[1], 3);
                    ByteArray array(buffer, 4);
                    magic = array.getUInt32(0, false);
                }
                else {
                    Poco::Logger::get("StreamReceiver").debug("Bytes of magic not received : disconnect and reconnect", __FILE__, __LINE__);
                    stream.close();
                    stream = server->acceptConnection();
                    stream.setKeepAlive(true);
                    continue;
                }
            }
            
            // -- infos
            if (magic == 0x4e10b1be) {

                Poco::Logger::get("StreamReceiver").debug("Header received", __FILE__, __LINE__);
                timestampStart = Poco::Timestamp().epochMicroseconds();
                while (stream.available() < 4 && !nextLoop) {
                    Timer::crossUsleep(250);
                    long deltaTime = Poco::Timestamp().epochMicroseconds(); - timestampStart;
                    if (deltaTime > 100000) {
                        nextLoop = true;
                    }
                }
                if (!nextLoop) {
                    
                    nbBytes = stream.receiveBytes(buffer, 4);
                    ByteArray array(buffer, 4);
                    uint32_t length = array.getUInt32(0, false);

                    timestampStart = Poco::Timestamp().epochMicroseconds();
                    while (stream.available() < length && !nextLoop) {
                        long deltaTime = Poco::Timestamp().epochMicroseconds(); - timestampStart;
                        if (deltaTime > 100000) {
                            nextLoop = true;
                        }
                    }
                    if (!nextLoop) {
                        Poco::Logger::get("StreamReceiver").debug("Data of header received", __FILE__, __LINE__);
                        nbBytes = stream.receiveBytes(buffer, length);
                        ByteArray array2(buffer, length);
                        uint32_t version = array2.getUInt32(0, false);
                        uint32_t format = array2.getUInt32(4, false);
                        
                        if (version == 1) {lutApplied = true;}
                        else {lutApplied = false;}
                    }
                    else {
                        Poco::Logger::get("StreamReceiver").debug("Data of header not received : disconnect and reconnect", __FILE__, __LINE__);
                        stream.close();
                        stream = server->acceptConnection();
                        stream.setKeepAlive(true);
                        continue;
                    }
                }
                else {
                    Poco::Logger::get("StreamReceiver").debug("Length not received : disconnect and reconnect", __FILE__, __LINE__);
                    stream.close();
                    stream = server->acceptConnection();
                    stream.setKeepAlive(true);
                    continue;
                }
            }

            // -- image
            else if (magic == 0xb17eb1be) {

                Poco::Logger::get("StreamReceiver").debug("Image received", __FILE__, __LINE__);
                timestampStart = Poco::Timestamp().epochMicroseconds();
                while (stream.available() < 4 && !nextLoop) {
                    long deltaTime = Poco::Timestamp().epochMicroseconds() - timestampStart;
                    if (deltaTime > 100000) {
                        nextLoop = true;
                    }
                }
                if (!nextLoop) {
                    Poco::Logger::get("StreamReceiver").debug("Length received", __FILE__, __LINE__);
                    nbBytes = stream.receiveBytes(buffer, 4);
                    ByteArray array(buffer, 4);
                    uint32_t length = array.getUInt32(0, false);

                    int bytesRead = 0;
                    bool skip = false;

                    Poco::Logger::get("StreamReceiver").debug("Start read image : " + std::to_string(length), __FILE__, __LINE__);

                    uint64_t timestampStartImage = Poco::Timestamp().epochMicroseconds();
                    while (bytesRead < length && !skip) {

                        int available = stream.available();
                        if (available > 0) {

                            if (length - bytesRead < available) {
                                bytesRead += stream.receiveBytes(&buffer[bytesRead], length - bytesRead);
                            }
                            else {
                                bytesRead += stream.receiveBytes(&buffer[bytesRead], available);
                            }
                        }
                        else {
                            long deltaTime = Poco::Timestamp().epochMicroseconds() - timestampStartImage;
                            if (deltaTime > 100000) {
                                skip = true;
                                break;
                            }
                        }
                    }

                    if (!skip) {

                        Poco::Logger::get("StreamReceiver").debug("Image received saved !", __FILE__, __LINE__);
                        // -- BGRA to RGBA
                        unsigned char* data = new unsigned char[720*576*4];
                        for (int i = 0; i < 720*576; i++) {
                            data[i*4] = buffer[i*4+2];
                            data[i*4+1] = buffer[i*4+1];
                            data[i*4+2] = buffer[i*4];
                            data[i*4+3] = buffer[i*4+3];
                        }
                        Image* img = new Image(720, 576, PNG_FORMAT_RGBA, data);
                        std::shared_ptr<Image> imgPtr = std::shared_ptr<Image>(img);
                        while (!mutexImages.tryLock()) {Timer::crossUsleep(20);}
                        images.push(imgPtr);
                        imgPtr.reset();
                        currentFrame++;
                        if (currentFrame > 10000000) {
                            currentFrame = 0;
                        }
                        mutexImages.unlock();
                    }
                    else {
                        Poco::Logger::get("StreamReceiver").debug("Image not completely received : " + std::to_string(bytesRead), __FILE__, __LINE__);
                        stream.close();
                        stream = server->acceptConnection();
                        stream.setKeepAlive(true);
                        continue;
                    }
                }
                else {
                    Poco::Logger::get("StreamReceiver").debug("Length not received : disconnect and reconnect", __FILE__, __LINE__);
                    stream.close();
                    stream = server->acceptConnection();
                    stream.setKeepAlive(true);
                    continue;
                }
            }
        }
    }
}