#include "ReliquaryDependencyAlreadyAdded.h"

namespace Arca
{
    ReliquaryDependencyAlreadyAdded::ReliquaryDependencyAlreadyAdded(const std::string& name) :
        Exception("Reliquary dependency name has already been added (" + name + ").")
    {}

    ReliquaryDependencyAlreadyAdded::ReliquaryDependencyAlreadyAdded(Reliquary& reliquary) :
        Exception("Reliquary dependency has already been added.")
    {}
}