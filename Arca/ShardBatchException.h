#pragma once

#include <stdexcept>

namespace Arca
{
    class ShardBatchNotSetup final : public std::runtime_error
    {
    public:
        ShardBatchNotSetup();
    };
}