// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NinjaInputFunctionLibrary.generated.h"

class UNinjaInputManagerComponent;
/**
 * Provides static helpers for the Ninja Input system.
 */
UCLASS()
class NINJAINPUT_API UNinjaInputFunctionLibrary : public UBlueprintFunctionLibrary
{
    
    GENERATED_BODY()

public:

    /**
     * Retrieves the Input Manager Component from a given actor.
     *
     * @param Actor
     *      The actor that may provide the Manager Component. Null values are handled.
     *
     * @return
     *      Input Manager Component assigned to the actor.
     */
    UFUNCTION(BlueprintPure, Category = "Ninja Input")
    static UNinjaInputManagerComponent* GetInputManagerComponent(const AActor* Actor);

    /**
     * Provides the Input Buffer assigned to a given actor.
     *
     * The generic Actor Component is guaranteed to be a valid implementation of the
     * Input Buffer interface and is also guaranteed to be usable.
     *
     * @param Actor
     *      The actor that may provide the Manager Component. Null values are handled.
     *
     * @return
     *      Actor Component that is a valid, enabled, implementation of the Input Buffer
     *      interface. May be null, so make sure to check before using it!
     */
    UFUNCTION(BlueprintPure, Category = "Ninja Input")
    static UActorComponent* GetInputBufferComponent(const AActor* Actor);
    
};
