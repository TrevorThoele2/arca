#pragma once

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Curator
    {
    protected:
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
        
        virtual ~Curator() = 0;

        Curator& operator=(const Curator& arg) = delete;
        Curator& operator=(Curator&& arg) = delete;

        void PostConstruct(Reliquary& owner);
        void Initialize();

        void Work(Stage& stage);
    protected:
        Curator() = default;
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    protected:
        virtual void PostConstructImplementation();
        virtual void InitializeImplementation();

        virtual void WorkImplementation(Stage& stage);
    private:
        Reliquary* owner = nullptr;
    private:
        INSCRIPTION_ACCESS;
        friend Reliquary;
        friend class ReliquaryCurators;
    };
}