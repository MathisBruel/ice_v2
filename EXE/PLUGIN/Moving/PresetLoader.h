#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <cstdlib>

#include "MovingHandler.h"

class PresetLoader
{

public:
    
    PresetLoader(std::string inDirectory);
    ~PresetLoader();

    void loadPresets();
    std::string savePresets(MovingHandler* handler, bool isColor = false);

    std::string getPositionPresetsString();
    std::string getColorPresetsString();

    MovingHandler* getHandlerPositionWithIndex(int index);
    MovingHandler* getHandlerColorWithIndex(int index);

    int getNbPresetPos() {return presetsPosition.size();}
    int getNbPresetColor() {return presetsColor.size();}

    std::string getLoadingError() {return errorLoading;}

private:

    std::string errorLoading;

    std::string directory;
    std::map<std::string, MovingHandler*> presetsPosition;
    std::map<std::string, MovingHandler*> presetsColor;
};