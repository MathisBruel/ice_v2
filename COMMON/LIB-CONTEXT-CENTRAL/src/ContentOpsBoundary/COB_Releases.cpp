#include "ContentOpsBoundary/COB_Releases.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_Releases::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<releases>";
    
    if (!this->empty()) {
        xmlStream << "\n";
        std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_Release& release) {
            std::ostringstream line;
            line << "  " << static_cast<std::string>(release) << "\n";
            return line.str();
        });
    }

    xmlStream << "</releases>";
    return xmlStream.str();
}