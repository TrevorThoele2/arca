#include "CuratorDependencyNotFound.h"

namespace Arca
{
    CuratorDependencyNotFound::CuratorDependencyNotFound() :
        Exception("A dependency for a Curator was not found.")
    {}
}