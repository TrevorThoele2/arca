#pragma once

#include "RelicID.h"
#include "Type.h"

namespace Arca
{
    enum class HandleObjectType
    {
        Relic,
        Shard
    };

    class HandleSlim
    {
    public:
        HandleSlim() = default;
        HandleSlim(RelicID id, Type type, HandleObjectType objectType);

        bool operator==(const HandleSlim& arg) const;
        bool operator!=(const HandleSlim& arg) const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Type Type() const;
        [[nodiscard]] HandleObjectType ObjectType();
    private:
        RelicID id = 0;
        Arca::Type type;
        HandleObjectType objectType = HandleObjectType::Relic;
    };
}