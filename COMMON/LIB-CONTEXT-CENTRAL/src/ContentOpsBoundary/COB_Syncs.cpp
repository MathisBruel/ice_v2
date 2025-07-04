#include "ContentOpsBoundary/COB_Syncs.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_Syncs::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<syncs>\n";
    std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_Sync& sync) {
        std::ostringstream line;
        line << "  " << static_cast<std::string>(sync) << "\n";
        return line.str();
    });
    xmlStream << "</syncs>";
    return xmlStream.str();
} 