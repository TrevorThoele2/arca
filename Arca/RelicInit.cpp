#include "RelicInit.h"

namespace Arca
{
    RelicInit::RelicInit(RelicID id, Reliquary& owner, ReliquaryShards& shards) : id(id), owner(owner), shards(&shards)
    {}
}