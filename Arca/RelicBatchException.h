#pragma once

#include <stdexcept>

namespace Arca
{
    class RelicBatchInvalidated final : public std::runtime_error
    {
    public:
        RelicBatchInvalidated();
    };
}