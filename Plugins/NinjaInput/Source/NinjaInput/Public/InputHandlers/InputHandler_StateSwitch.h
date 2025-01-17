// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "NinjaInputHandler.h"
#include "InputHandler_StateSwitch.generated.h"

/**
 * Implements a switch that will be inverted everytime a trigger happens.
 *
 * It's connected to the Input State Switch Interface, which is used to retrieve a
 * given state, for a given Gameplay Tag and then, alternate it properly.
 *
 * This switch can be used in two ways:
 *
 * TOGGLE: If you provide an input with a single trigger such as Pressed, Hold, Tap
 * and so on, then every time this input triggers, it will invert the state switch.
 * Think about it as a "Caps Lock" key operation.
 *
 * MOMENTARY: If you provide an input with two triggers, such as Pressed and Released,
 * then every one of these triggers will invert the state switch. Think about it as
 * the "Shift" key operation.
 */
UCLASS(meta = (DisplayName = "Utility: State Switch"))
class NINJAINPUT_API UInputHandler_StateSwitch : public UNinjaInputHandler
{
    
	GENERATED_BODY()

public:

    UInputHandler_StateSwitch();

protected:

    /** Gameplay Tag representing the state managed by this switch. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Switch")
    FGameplayTag StateSwitchTag;

    virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
        const FInputActionValue& Value, const UInputAction* InputAction) const override;
    
    /**
     * Checks if the state is active, by invoking the interface on the manager's owner.
     *
     * @param Manager       Input Manager Component used for evaluation.
     * @return              True if the state tracked by this switch is active/engaged.
     */
    UFUNCTION(BlueprintPure, Category = "State Switch Handler")	
    bool IsActive(const UNinjaInputManagerComponent* Manager) const;

    /**
     * Determines a new on/off value for the state.
     *
     * @param Manager       Input Manager Component used for to invoke the interface.
     * @param NewState      New State value to be assigned.
     */
    UFUNCTION(BlueprintCallable, Category = "State Switch Handler")	
    void SetSwitchState(const UNinjaInputManagerComponent* Manager, bool NewState) const;
    
};
