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
};

namespace Arca
{
    template<>
    struct Traits<MatrixTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardTestsShard";
    };
}

namespace Inscription
{
    template<>
    class Scribe<MatrixTestsFixture::Shard, BinaryArchive> final :
        public ArcaNullScribe<MatrixTestsFixture::Shard, BinaryArchive>
    {};
}