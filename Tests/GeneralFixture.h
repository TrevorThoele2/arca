#pragma once

#include <TestFramework/DataGeneration.h>

#include <Arca/Reliquary.h>
#include <Arca/TypeRegistration.h>

#include <Inscription/OutputBinaryArchive.h>
#include <Inscription/InputBinaryArchive.h>

class GeneralFixture
{
public:
    ::TestFramework::DataGeneration dataGeneration;

    ::Arca::TypeRegistration typeRegistration;
public:
    GeneralFixture();

    template<class T>
    T CreateRegistered();
    template<>
    ::Arca::Reliquary CreateRegistered();
    template<>
    ::Inscription::OutputBinaryArchive CreateRegistered();
    template<>
    ::Inscription::InputBinaryArchive CreateRegistered();
};

template<class T>
T GeneralFixture::CreateRegistered()
{
    static_assert(false, "CreateRegistered cannot be called generically. Specialize it or call a different specialization.");
}

template<>
::Arca::Reliquary GeneralFixture::CreateRegistered()
{
    ::Arca::Reliquary reliquary;
    typeRegistration.PushAllTo(reliquary);
    return reliquary;
}

template<>
::Inscription::OutputBinaryArchive GeneralFixture::CreateRegistered()
{
    ::Inscription::OutputBinaryArchive archive("Test.dat", "testing", 1);
    typeRegistration.PushAllTo(archive);
    return archive;
}

template<>
::Inscription::InputBinaryArchive GeneralFixture::CreateRegistered()
{
    ::Inscription::InputBinaryArchive archive("Test.dat", "testing");
    typeRegistration.PushAllTo(archive);
    return archive;
}