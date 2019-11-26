#pragma once

#include "Exception.h"

namespace Arca
{
    class InvalidPipeline final : public Exception
    {
    public:
        explicit InvalidPipeline(const std::string& reason);
    };
}