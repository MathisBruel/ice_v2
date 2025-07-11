#include "ContentOpsBoundary/COB_SyncLoops.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_SyncLoops::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<syncLoops>";
    if (!this->empty()) {
        xmlStream << "\n";
        std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_SyncLoop& syncLoop) {
            std::ostringstream line;
            line << "  " << static_cast<std::string>(syncLoop) << "\n";
            return line.str();
        });
    }
    xmlStream << "</syncLoops>";
    return xmlStream.str();
} 