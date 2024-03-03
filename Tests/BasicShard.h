#pragma once

#include <Arca/Shard.h>

class BasicShard final
{
public:
    int integer = 0;
    std::string string;
public:
    explicit BasicShard() = default;
    explicit BasicShard(int integer, const std::string& string);
};

namespace Arca
{
    template<>
    struct Traits<BasicShard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "BasicShard";
    };
}

namespace Inscription
{
    template<>
    class Scribe<BasicShard> final
    {
    public:
        using ObjectT = BasicShard;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("integer", object.integer);
            archive("string", object.string);
        }
    };

    template<class Archive>
    struct ScribeTraits<BasicShard, Archive>
    {
        using Category = ArcaCompositeScribeCategory<BasicShard>;
    };
}