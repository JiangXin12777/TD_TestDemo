// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "NinjaInputFunctionLibrary.h"

#include "NinjaInputManagerComponent.h"

UNinjaInputManagerComponent* UNinjaInputFunctionLibrary::GetInputManagerComponent(const AActor* Actor)
{
    if (IsValid(Actor))
    {
        return Actor->FindComponentByClass<UNinjaInputManagerComponent>();
    }

    return nullptr;
}

UActorComponent* UNinjaInputFunctionLibrary::GetInputBufferComponent(const AActor* Actor)
{
    if (IsValid(Actor))
    {
        const TArray<UActorComponent*> Components = Actor->GetComponentsByInterface(UInputBufferInterface::StaticClass());

        const int32 Idx = Components.IndexOfByPredicate([](const UActorComponent* Candidate)
            { return IsValid(Candidate) && IInputBufferInterface::Execute_IsInputBufferEnabled(Candidate); });

        return Idx != INDEX_NONE ? Components[Idx] : nullptr;
    }

    return nullptr;
}
