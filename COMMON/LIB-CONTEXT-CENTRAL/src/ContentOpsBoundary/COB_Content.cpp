#include "ContentOpsBoundary/COB_Content.h"
#include <string>

COB_Content::COB_Content(int contentId, std::string contentTitle)
    : COD_Content(contentId, contentTitle)
{
}

COB_Content::operator std::string() const
{
    std::string xml = "<content";
    xml += " id_content=\"" + std::to_string(this->GetContentId()) + "\"";
    xml += " title=\"" + this->GetContentTitle() + "\"";
    xml += "/>";
    return xml;
} 