#include "CuratorProvider.h"

namespace Arca
{
    CuratorProviderBase::~CuratorProviderBase() = default;

    auto CuratorProviderBase::Provide(CuratorMap& curators, CuratorProviderMap& curatorProviders, Reliquary& reliquary) -> Provided
    {
        auto typeHandle = typeDescription.typeHandle;

        {
            const auto found = curators.find(typeHandle);
            if (found != curators.end())
                return { found->second, typeDescription };
        }

        auto provided = ProvideImpl(curators, curatorProviders, reliquary);

        {
            auto curator = (provided.index() == 0)
                ? std::get<0>(provided).get()
                : std::get<1>(provided);
            curators.emplace(typeHandle, curator);
        }

        return
        {
            ProvideImpl(curators, curatorProviders, reliquary),
            typeDescription
        };
    }

    CuratorProviderBase::CuratorProviderBase(CuratorTypeDescription typeDescription) : typeDescription(std::move(typeDescription))
    {}

    ExternalCuratorProvider::ExternalCuratorProvider(Curator* external, CuratorTypeDescription typeDescription) :
        CuratorProviderBase(std::move(typeDescription)), external(external)
    {}

    auto ExternalCuratorProvider::ProvideImpl(
        CuratorMap& curators, CuratorProviderMap& curatorProviders, Reliquary& reliquary) -> ProvidedCurator
    {
        return external;
    }
}