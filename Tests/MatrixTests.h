#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>

using namespace Arca;

class MatrixTestsFixture : public GeneralFixture
{
public:
    struct Shard
    {
        int myInt;
    };

    struct OtherShard
    {
        std::string string;
    };
};

namespace Arca
{
    template<>
    struct Traits<MatrixTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "ShardTestsShard"; }
    };

    template<>
    struct Traits<MatrixTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "ShardTestsOtherShard"; }
    };
}

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<MatrixTestsFixture::Shard, Archive> final
    {
        using Category = ArcaNullScribeCategory<MatrixTestsFixture::Shard>;
    };

    template<class Archive>
    struct ScribeTraits<MatrixTestsFixture::OtherShard, Archive> final
    {
        using Category = ArcaNullScribeCategory<MatrixTestsFixture::OtherShard>;
    };
}