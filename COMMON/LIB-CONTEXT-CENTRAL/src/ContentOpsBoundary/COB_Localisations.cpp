#include "ContentOpsBoundary/COB_Localisations.h"
#include <sstream>
#include <iterator>
#include <algorithm>

COB_Localisations::operator std::string() const
{
    std::ostringstream xmlStream;
    xmlStream << "<localisations>\n";
    
    std::transform(this->begin(), this->end(), std::ostream_iterator<std::string>(xmlStream), [](const COB_Localisation& localisation) {
        std::ostringstream line;
        line << "  " << static_cast<std::string>(localisation) << "\n";
        return line.str();
    });

    xmlStream << "</localisations>";
    return xmlStream.str();
}