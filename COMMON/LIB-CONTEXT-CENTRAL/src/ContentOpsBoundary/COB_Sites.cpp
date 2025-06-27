#include "ContentOpsBoundary/COB_Sites.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_Sites::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<sites>\n";
    
    std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_Site& site) {
        std::ostringstream line;
        line << "  " << static_cast<std::string>(site) << "\n";
        return line.str();
    });
    
    xmlStream << "</sites>";
    return xmlStream.str();
}