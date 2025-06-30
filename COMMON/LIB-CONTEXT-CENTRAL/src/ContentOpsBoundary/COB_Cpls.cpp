#include "ContentOpsBoundary/COB_Cpls.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_Cpls::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<cpls>";
    
    if (!this->empty()) {
        xmlStream << "\n";
        std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_Cpl& cpl) {
            std::ostringstream line;
            line << "  " << static_cast<std::string>(cpl) << "\n";
            return line.str();
        });
    }

    xmlStream << "</cpls>";
    return xmlStream.str();
} 