#pragma once

#include <Chroma/StringUtility.h>
#include <optional>

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