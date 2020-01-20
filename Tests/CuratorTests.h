#pragma once

#include "GeneralFixture.h"

#include <Arca/Curator.h>

#include <Inscription/BinaryArchive.h>
#include "Chroma/StringUtility.h"

using namespace Arca;

class CuratorTestsFixture : public GeneralFixture
{
public:
    class BasicCurator final : public Curator
    {
    public:
        bool isInitialized = false;

        bool shouldAbort = false;
        std::function<void()> onPostConstruct = [](){};
        std::function<void()> onWork = [](){};

        [[nodiscard]] Reliquary& OwnerFromOutside();
        [[nodiscard]] const Reliquary& OwnerFromOutside() const;
    public:
        int value = 0;
    protected:
        void PostConstructImplementation() override;
        void InitializeImplementation() override;
        void WorkImplementation(Stage& stage) override;
    };

    class OtherBasicCurator final : public Curator
    {
    public:
        int value = 0;
    };

    template<size_t id>
    class DifferentiableCurator final : public Curator
    {
    public:
        static std::vector<Curator*>* postConstructCheckpoints;
        static std::vector<Curator*>* initializationCheckpoints;
        static std::vector<Curator*>* workCheckpoints;

        bool shouldAbort = false;
    public:
        void PostConstructImplementation() override
        {
            if (postConstructCheckpoints)
                postConstructCheckpoints->push_back(this);
        }

        void InitializeImplementation() override
        {
            if (initializationCheckpoints)
                initializationCheckpoints->push_back(this);
        }

        void WorkImplementation(Stage& stage) override
        {
            if (workCheckpoints)
                workCheckpoints->push_back(this);

            if (shouldAbort)
                stage.Abort();
        }
    };

    class CuratorWithNonDefaultConstructor final : public Curator
    {
    public:
        int myValue = 0;

        explicit CuratorWithNonDefaultConstructor(int myValue);
    };
};

template<size_t id>
std::vector<Curator*>* CuratorTestsFixture::DifferentiableCurator<id>::postConstructCheckpoints = nullptr;

template<size_t id>
std::vector<Curator*>* CuratorTestsFixture::DifferentiableCurator<id>::initializationCheckpoints = nullptr;

template<size_t id>
std::vector<Curator*>* CuratorTestsFixture::DifferentiableCurator<id>::workCheckpoints = nullptr;

namespace Arca
{
    template<size_t id>
    struct Traits<CuratorTestsFixture::DifferentiableCurator<id>>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "DifferentiableCurator" + Chroma::ToString(id);
    };
}