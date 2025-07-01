#include "ContentOpsBoundary/COB_Types.h"

#include <sstream>
#include <iterator>
#include <algorithm>

COB_Types::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<types>\n";
    
    std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_Type& type) {
        std::ostringstream line;
        line << "  " << static_cast<std::string>(type) << "\n";
        return line.str();
    });

    xmlStream << "</types>";
    return xmlStream.str();
}