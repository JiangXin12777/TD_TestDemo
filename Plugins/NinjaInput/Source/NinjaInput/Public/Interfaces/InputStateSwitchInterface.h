// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "InputStateSwitchInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInputStateSwitchInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Represents a toggle or momentary state that can be managed by input.
 */
class NINJAINPUT_API IInputStateSwitchInterface
{
	
    GENERATED_BODY()

public:

    /**
     * Determines the current state switch's value.
     *
     * @param SwitchStateTag    GameplayTag representing the state.
     * @return                  Boolean value representing the current state.
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input State Switch Interface")
    bool GetSwitchState(FGameplayTag SwitchStateTag) const;

    /**
     * Provides a new value for a state switch.
     *
     * @param SwitchStateTag    GameplayTag representing the state.
     * @param NewSwitchValue    New value for this state switch.
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input State Switch Interface")
    void SetSwitchState(FGameplayTag SwitchStateTag, bool NewSwitchValue);
	
};