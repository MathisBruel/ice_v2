#include <cstdlib>
#include <iostream>

#pragma once

#include "image.h"

class OutputPanelImage
{

public:

    OutputPanelImage(std::string name, std::shared_ptr<Image> out) {this->name = name; this->out = out;}
    ~OutputPanelImage() {out.reset();}

    std::string getName() {return name;}
    std::shared_ptr<Image> getOut() {return out;}

private:

    std::string name;
    std::shared_ptr<Image> out;
};