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
        static inline const TypeName typeName = "ShardTestsShard";
    };

    template<>
    struct Traits<MatrixTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardTestsOtherShard";
    };
}

namespace Inscription
{
    template<>
    class Scribe<MatrixTestsFixture::Shard, BinaryArchive> final :
        public ArcaNullScribe<MatrixTestsFixture::Shard, BinaryArchive>
    {};

    template<>
    class Scribe<MatrixTestsFixture::OtherShard, BinaryArchive> final :
        public ArcaNullScribe<MatrixTestsFixture::OtherShard, BinaryArchive>
    {};
}