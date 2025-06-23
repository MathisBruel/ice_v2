#pragma once
#include "BoundaryStateManager.h"
#include <stdexcept>

using namespace ContentOpsBoundaryEnum;

class BoundaryManager {
    public:
    std::string GetAllContentsAsXml();
    std::string GetContentAsXml(int contentId);
    void UpdateContent(int contentId);
    void CreateContent();

    static BoundaryManager& GetInstance() {
        static BoundaryManager instance;
        return instance;
    }

    BoundaryManager(const BoundaryManager&) = delete;
    BoundaryManager& operator=(const BoundaryManager&) = delete;

    private:
    BoundaryManager() = default;
    
};