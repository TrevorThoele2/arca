#pragma once

#include "CuratorInit.h"
#include "HandledCommands.h"

#include "MutablePointer.h"

#include "RelicIndex.h"
#include "ShardIndex.h"
#include "GlobalIndex.h"
#include "MatrixIndex.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Curator
    {
    public:
        class Stage
        {
        public:
            Stage() = default;
            void Abort();
            [[nodiscard]] bool IsAborted() const;
        private:
            bool isAborted = false;
        };
    public:
        Curator(const Curator& arg) = delete;
        Curator(Curator&& arg) = delete;
        
        Curator& operator=(const Curator& arg) = delete;
        Curator& operator=(Curator&& arg) = delete;
    protected:
        using Init = CuratorInit;
        explicit Curator(Init init);
    protected:
        [[nodiscard]] MutablePointer MutablePointer();
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    private:
        Reliquary* owner = nullptr;
    private:
        INSCRIPTION_ACCESS;
        friend class ReliquaryCurators;
    };
}
