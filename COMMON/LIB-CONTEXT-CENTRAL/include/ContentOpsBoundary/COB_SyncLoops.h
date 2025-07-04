#pragma once
#include <vector>
#include "ContentOpsBoundary/COB_SyncLoop.h"

class COB_SyncLoops : public std::vector<COB_SyncLoop>
{
public:
    operator std::string() const;
}; 