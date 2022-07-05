#include "udpCommandReceiver.h"

UdpCommandReceiver::UdpCommandReceiver(std::string address, uint16_t port)
{
    Poco::Logger::get("UdpCommandReceiver").debug("UDP client opened with address and port " + address + ":" + std::to_string(port),  __FILE__, __LINE__);
    Poco::Net::SocketAddress socket = Poco::Net::SocketAddress(address, port);

    try {
        receiver = new Poco::Net::DatagramSocket(socket, false);
    } catch (std::exception &e) {
        Poco::Logger::get("UdpCommandReceiver").error("Error while opening receiver socket", __FILE__, __LINE__);
    }

    stop = false;
    thread = nullptr;

    thread = new Poco::Thread();
	thread->start(*this);
}
UdpCommandReceiver::~UdpCommandReceiver()
{
    if (receiver != nullptr) {
        receiver->close();
        delete receiver;
    }

    stop = true;
	if (thread != nullptr)
	{
		thread->join();
		delete thread;
	}
}
std::shared_ptr<Command> UdpCommandReceiver::getFirstCommand()
{
    std::shared_ptr<Command> cmd = nullptr;
    while (!commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    std::map<std::string, std::shared_ptr<Command>>::iterator cmdIt = commands.begin();
    if (cmdIt != commands.end()) {
        cmd = cmdIt->second;
    }
    commandsMutex.unlock();
    return cmd;
}

void UdpCommandReceiver::deleteCommand(std::string uuid)
{
    while (!commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    commands.erase(uuid);
    commandsMutex.unlock();
}
void UdpCommandReceiver::addCommand(std::shared_ptr<Command> command)
{
    while (!commandsMutex.tryLock()) {Timer::crossUsleep(20);}
    commands.insert_or_assign(command->getUuid(), command);
    commandsMutex.unlock();
}

void UdpCommandReceiver::run()
{
    Poco::Timespan span(5000);
    while (!stop) 
    {
        if (receiver->poll(span, Poco::Net::Socket::SELECT_READ))
        {
            Poco::Logger::get("UdpCommandReceiver").debug("Receiving !", __FILE__, __LINE__);
            uint8_t buffer[2048];
            int nbBytes = receiver->receiveBytes(buffer, sizeof(buffer));
            if (nbBytes > 0) {
                handleResponse(buffer, nbBytes);
            }
        }
    }
}

void UdpCommandReceiver::handleResponse(uint8_t * buffer, uint32_t length)
{
    std::string command((char*)buffer, std::size_t(length));
    Poco::Logger::get("UdpCommandReceiver").debug(command, __FILE__, __LINE__);
    std::shared_ptr<Command> cmd = nullptr;

    // -- cut response
    int posSep = command.find(" ", 0);
    if (posSep == std::string::npos) {
        // -- no parameters command
        if (command == "UNFORCE_ALL_DEVICES") {
            cmd = std::make_shared<Command>(Command(Command::UNFORCE_ALL_DEVICES));
        }
        else if (command == "STOP_SCRIPT") {
            cmd = std::make_shared<Command>(Command(Command::STOP_SCRIPT));
        }
        else if (command == "SET_PAR_SCENE_ON") {
            cmd = std::make_shared<Command>(Command(Command::SET_PAR_SCENE_ON));
        }
        else if (command == "SET_PAR_SCENE_OFF") {
            cmd = std::make_shared<Command>(Command(Command::SET_PAR_SCENE_OFF));
        }
        else if (command == "SET_PAR_AMBIANCE_ON") {
            cmd = std::make_shared<Command>(Command(Command::SET_PAR_AMBIANCE_ON));
        }
        else if (command == "SET_PAR_AMBIANCE_OFF") {
            cmd = std::make_shared<Command>(Command(Command::SET_PAR_AMBIANCE_OFF));
        }
    }
    else {
        std::string action = command.substr(0, posSep);

        if (action == "UNFORCE_DEVICE") {
            std::string deviceStr = command.substr(posSep+1);
            cmd = std::make_shared<Command>(Command(Command::UNFORCE_DEVICE));
            cmd->addParameter("device", deviceStr);
        }
        else if (action == "SWITCH_MODE") {
            std::string modeStr = command.substr(posSep+1);
            cmd = std::make_shared<Command>(Command(Command::SWITCH_MODE));
            cmd->addParameter("mode", modeStr);
        }
        else if (action == "LAUNCH_SCRIPT") {
            
            cmd = std::make_shared<Command>(Command(Command::LAUNCH_SCRIPT));

            // -- check if force
            std::string paramStr = command.substr(posSep+1);
            int posSep2 = paramStr.find(" ", 0);
            if (posSep2 == std::string::npos) {
                cmd->addParameter("script", paramStr);
            }
            else {
                std::string scriptStr = paramStr.substr(0, posSep2);
                std::string forceStr = paramStr.substr(posSep2+1);
                cmd->addParameter("script", scriptStr);
                if (forceStr == "force") {
                    cmd->addParameter("force", "true");
                }
            }
        }
        else if (action == "SWITCH_PROJECTION") {
            std::string projectionStr = command.substr(posSep+1);
            cmd = std::make_shared<Command>(Command(Command::SWITCH_PROJECTION));
            cmd->addParameter("projection", projectionStr);
        }
    }

    if (cmd != nullptr && cmd->validateCommand()) {
        cmd->setDispatch();
        commands.insert_or_assign(cmd->getUuid(), cmd);
    }
}