#include "ContentOpsBoundary/COB_ServerPairs.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_ServerPairs::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<servPairs>";
    if (!this->empty()) {
        xmlStream << "\n";
        std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_ServerPair& serverPair) {
            std::ostringstream line;
            line << "  " << static_cast<std::string>(serverPair) << "\n";
            return line.str();
        });
    }
    xmlStream << "</servPairs>";
    return xmlStream.str();
} 