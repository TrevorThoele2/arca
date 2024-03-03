#pragma once

#include <optional>
#include <Chroma/StringUtility.h>

namespace Arca
{
    enum class ObjectType
    {
        Shard,
        Relic,
        Matrix,
        Curator,
        Signal,
        Command
    };
}

namespace Chroma
{
    template<>
    std::optional<Arca::ObjectType> FromString(const std::string& arg);
    std::string ToString(Arca::ObjectType objectType);
}