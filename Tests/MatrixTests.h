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
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "ShardTestsShard";
    };

    template<>
    struct Traits<MatrixTestsFixture::OtherShard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "ShardTestsOtherShard";
    };
}

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<MatrixTestsFixture::Shard, Format> final
    {
        using Category = ArcaNullScribeCategory<MatrixTestsFixture::Shard>;
    };

    template<class Format>
    struct ScribeTraits<MatrixTestsFixture::OtherShard, Format> final
    {
        using Category = ArcaNullScribeCategory<MatrixTestsFixture::OtherShard>;
    };
}