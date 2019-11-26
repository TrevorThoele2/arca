#include "PipelineException.h"

namespace Arca
{
    InvalidPipeline::InvalidPipeline(const std::string& reason) :
        Exception(reason)
    {}
}