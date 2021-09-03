#include "ObjectType.h"

namespace Chroma
{
    template<>
    std::optional<Arca::ObjectType> FromString(const std::string& arg)
    {
        if (arg == "shard")
            return Arca::ObjectType::Shard;
        else if (arg == "relic")
            return Arca::ObjectType::Relic;
        else if (arg == "matrix")
            return Arca::ObjectType::Matrix;
        else if (arg == "curator")
            return Arca::ObjectType::Curator;
        else if (arg == "signal")
            return Arca::ObjectType::Signal;
        else
            return {};
    }

    std::string ToString(Arca::ObjectType objectType)
    {
        switch (objectType)
        {
        case Arca::ObjectType::Shard:
            return "shard";
        case Arca::ObjectType::Relic:
            return "relic";
        case Arca::ObjectType::Matrix:
            return "matrix";
        case Arca::ObjectType::Curator:
            return "curator";
        case Arca::ObjectType::Signal:
            return "signal";
        case Arca::ObjectType::Command:
            return "command";
        default:
            return "";
        }
    }
}