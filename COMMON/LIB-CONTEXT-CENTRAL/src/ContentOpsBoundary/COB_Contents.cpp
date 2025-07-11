#include "ContentOpsBoundary/COB_Contents.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_Contents::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<contents>";
    
    if (!this->empty()) {
        xmlStream << "\n";
        std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_Content& content) {
                std::ostringstream line;
            line << "  " << static_cast<std::string>(content) << "\n";
            return line.str();
        });
    }
    
    xmlStream << "</contents>";
    return xmlStream.str();
} 