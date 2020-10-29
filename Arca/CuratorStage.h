#pragma once

namespace Arca
{
    class CuratorStage
    {
    public:
        CuratorStage() = default;
        void Abort();
        [[nodiscard]] bool IsAborted() const;
    private:
        bool isAborted = false;
    };
}