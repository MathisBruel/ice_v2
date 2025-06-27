#include "ContentOpsBoundary/COB_Group.h"
#include <string>

COB_Group::COB_Group(int groupId, std::string groupName, int groupParent)
    : COD_Group(groupId, groupName, groupParent)
{
}

COB_Group::operator std::string() const
{
    std::string xml = "<group";
    xml += " id_group=\"" + std::to_string(this->GetGroupId()) + "\"";
    if (this->GetGroupParent() != -1) {
        xml += " id_group_1=\"" + std::to_string(this->GetGroupParent()) + "\"";
    }
    xml += " name=\"" + this->GetGroupName() + "\"";
    xml += "/>";
    return xml;
} 