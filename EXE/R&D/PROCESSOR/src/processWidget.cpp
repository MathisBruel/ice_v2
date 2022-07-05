#include "processWidget.h"
#include <cuda.h>

ProcessWidget::ProcessWidget()
{
    try {
        Poco::Logger::root().setChannel(new Poco::ConsoleChannel());
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);
}

ProcessWidget::~ProcessWidget()
{
    
}

void ProcessWidget::initialize(Application& self) {
    Application::initialize(self);
}
void ProcessWidget::reinitialize(Application& self) {
    Application::reinitialize(self);
}
void ProcessWidget::uninitialize()
{
    Poco::Util::Application::uninitialize();
}

void ProcessWidget::parseScene()
{
    // -- load file
    std::ifstream in(fileScene);
    Poco::XML::InputSource src(in);

    // -- parse
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> doc;
    
    try {
        doc = parser.parse(&src);
    } catch (std::exception &e) {
        Poco::Logger::get("LutConfiguration").error("Exception when parsing XML file !", __FILE__, __LINE__);
        in.close();
        return;
    }
    in.close();

    Poco::XML::Node* node = doc->getNodeByPath("xmeml/sequence/media/video/track[@MZ.TrackTargeted='1']");
    Poco::XML::NodeList* clips = node->childNodes();
    for (int h = 0; h < clips->length(); h++) {
        Poco::XML::Node* itemClip = clips->item(h);
        Poco::XML::NodeList* clipItems = itemClip->childNodes();
        for (int i = 0; i < clipItems->length(); i++) {

            int start, end;
            if (clipItems->item(i)->nodeName() == "start") {
                start = std::stoi(clipItems->item(i)->innerText());
            }
            else if (clipItems->item(i)->nodeName() == "end") {
                end = std::stoi(clipItems->item(i)->innerText());
                sceneCuts.insert_or_assign(start, end);
            }
        }
        clipItems->release();
    }
    clips->release();
    doc->release();
}

int ProcessWidget::main(const std::vector<std::string> &arguments)
{
    for (int i = 0; i < arguments.size(); i++) {

        // -- arguments format "key:value"
        int posSep = arguments.at(i).find(":", 0);
        if (posSep == std::string::npos) {
            std::cout << "Options must be formatted as follow : \"key:value\"" << std::endl;
            continue;
        }

        std::string key = arguments.at(i).substr(0, posSep);
        std::string value = arguments.at(i).substr(posSep+1);

        // -- path to config files
        if (key == "fileIn") {fileIn = value;}
        else if (key == "fileScene") {fileScene = value;}
        else {std::cout << "Option " + key + " not recognized !" << std::endl;}
    }

    Poco::Path pathVideo(fileIn);
    if (pathVideo.getExtension() == "png") {
        Image* img = new Image();
        img->openImage(fileIn);
        Image* left = img->extractData(0, 0, img->getWidth()/2, img->getHeight());
        SeamProcessor processor(left);
        Image* compute = processor.seamCarvingAddToLeft(460, 1384);
        delete img;
        delete left;
        compute->saveImage("compute.png");
        delete compute;
    }
    else {
        parseScene();
        SeamProcessThread videoThread;
        videoThread.startThread(fileIn, sceneCuts);
        while (!videoThread.isFinished()) {
            Timer::crossUsleep(1000000);
        }
    }
}