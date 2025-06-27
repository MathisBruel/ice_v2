#include "ContentOpsBoundary/COB_Groups.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_Groups::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<groups>\n";
    
    std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_Group& group) {
        std::ostringstream line;
        line << "  " << static_cast<std::string>(group) << "\n";
        return line.str();
    });
    
    xmlStream << "</groups>";
    return xmlStream.str();
}