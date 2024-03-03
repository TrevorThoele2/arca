#include "MutablePointer.h"

namespace Arca
{
    MutablePointer::MutablePointer(ReliquaryRelics& relics, ReliquaryShards& shards) :
        relics(&relics), shards(&shards)
    {}
}