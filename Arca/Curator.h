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

        void Initialize(Reliquary& owner);
        void StartStep();
        void Work();
        void StopStep();

        [[nodiscard]] bool IsStarted() const;
    protected:
        Curator() = default;
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    protected:
        virtual void InitializeImplementation();
        virtual bool StartStepImplementation();
        virtual void WorkImplementation();
        virtual void StopStepImplementation();
    private:
        Reliquary* owner = nullptr;

        bool isStarted = false;
    private:
        INSCRIPTION_ACCESS;
        friend Reliquary;
    };
}