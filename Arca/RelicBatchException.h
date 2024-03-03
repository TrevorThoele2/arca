#pragma once

#include <stdexcept>

namespace Arca
{
    class RelicBatchNotSetup final : public std::runtime_error
    {
    public:
        RelicBatchNotSetup();
    };
}