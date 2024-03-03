#pragma once

#include "CuratorTraits.h"
#include "CuratorScribe.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Curator
    {
    public:
        Curator(const Curator& arg) = delete;
        Curator(Curator&& arg) = delete;
        
        virtual ~Curator() = 0;

        Curator& operator=(const Curator& arg) = delete;
        Curator& operator=(Curator&& arg) = delete;

        void StartStep();
        void Work();
        void StopStep();

        [[nodiscard]] bool IsStarted() const;
    protected:
        explicit Curator(Reliquary& owner);
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    protected:
        virtual bool StartStepImplementation();
        virtual void WorkImplementation();
        virtual void StopStepImplementation();
    private:
        Reliquary* owner;

        bool isStarted = false;
    private:
        INSCRIPTION_ACCESS;
        friend Reliquary;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::Curator, BinaryArchive> final :
        public CompositeScribe<::Arca::Curator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}