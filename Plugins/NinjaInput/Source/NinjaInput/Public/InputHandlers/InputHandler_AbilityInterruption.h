// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NinjaInputHandler.h"
#include "InputHandler_AbilityInterruption.generated.h"

/**
 * Basic Ability Interruption Handler.
 */
UCLASS(Abstract)
class NINJAINPUT_API UInputHandler_AbilityInterruption : public UNinjaInputHandler
{
    
    GENERATED_BODY()

public:

    UInputHandler_AbilityInterruption();
    
};
