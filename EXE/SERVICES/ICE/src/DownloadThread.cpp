#include "DownloadThread.h"

DownloadThread::DownloadThread()
{
    stop = false;
    thread = nullptr;
}
    
DownloadThread::~DownloadThread()
{
    if (thread != nullptr) {
        thread->join();
        delete thread;
    }
}

void DownloadThread::startThread()
{
    // -- start thread
    thread = new Poco::Thread("DownloadThread");
    thread->setPriority(Poco::Thread::Priority::PRIO_NORMAL);
    thread->start(*this);
}

void DownloadThread::run() {

    // -- work every 5 seconds
    int waitTime = 5000;
    Poco::Stopwatch watch;
    ApplicationContext* context = ApplicationContext::getCurrentContext();

    while (!stop) {
	    
        watch.reset();
        watch.start();

        CentralRepository *repoCentral = context->getRepoCentral();
        while (!repoCentral->tryLockDownloadMutex()) {usleep(20);}
        std::map<std::string, std::string> listDownloads(repoCentral->getFilesToDownloadList());
        repoCentral->unlockDownloadMutex();

        if (listDownloads.size() > 0) {
            std::string srcPath = listDownloads.begin()->first;
            std::string dstPath = listDownloads.begin()->second;

            if (repoCentral->openDownload()) {
                Poco::Logger::get("DownloadThread").information("Download file " + srcPath + " to " + dstPath, __FILE__, __LINE__);
                repoCentral->downloadFile(srcPath, dstPath);
                repoCentral->closeDownload();

                // -- exception check coherence cis
                Poco::Path pathFile(dstPath);
                Poco::File file(dstPath);

                // -- CIS
                if (pathFile.getExtension().find("cis") != std::string::npos) {
                    // -- check sha1
                    if (Converter::calculateSha1OfFile(pathFile.toString()) != pathFile.getBaseName()) {
                        Poco::Logger::get("DownloadThread").error("Cis file is corrupted : delete", __FILE__, __LINE__);
                        file.remove(false);
                    }
                }

                // -- TODO for HTTP : check SHA1
                else {
                    
                }

                Poco::Logger::get("DownloadThread").information("File downloaded" + dstPath, __FILE__, __LINE__);

                while (!context->tryLockLocalRepoMutex()) {usleep(20);}
                context->getRepoLocal()->updateScriptsInformations();
                context->unlockLocalRepoMutex();

                Poco::Logger::get("DownloadThread").information("Script updated" + dstPath, __FILE__, __LINE__);
                repoCentral->removeDownload(srcPath);
            }
        }

        long sleepTime = waitTime*1000-watch.elapsed();

        if (sleepTime > 0) {
            usleep(sleepTime);
        }
        else {
            Poco::Logger::get("DownloadThread").warning("Thread exceeding time of " + std::to_string(-sleepTime) + "µs", __FILE__, __LINE__);
        }
    }
}