// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_AbilityActivation.h"

#include "AbilitySystemComponent.h"
#include "NinjaInputManagerComponent.h"

UInputHandler_AbilityActivation::UInputHandler_AbilityActivation()
{
    bToggledActivation = false;
    bSendEventIfActive = false;
    bTriggerEventLocally = true;
    bTriggerEventOnServer = true;
    ActiveEventTag = FGameplayTag::EmptyTag;
    TriggerEvents.Add(ETriggerEvent::Triggered);
}

int32 UInputHandler_AbilityActivation::SendGameplayEvent(const UNinjaInputManagerComponent* Manager,
    const FGameplayTag GameplayEventTag, const FInputActionValue& Value, const UInputAction* InputAction) const
{
    check(IsValid(Manager));
    return Manager->SendGameplayEventToOwner(GameplayEventTag, Value, InputAction,
        bTriggerEventLocally, bTriggerEventOnServer);
}

void UInputHandler_AbilityActivation::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    if (Value.GetValueType() == EInputActionValueType::Boolean)
    {
        if (Value.Get<bool>())
        {
            if (!TryHandleActiveAbility(Manager, Value, InputAction))
            {
                ActivateAbility(Manager, Value, InputAction);
            }
        }
        else
        {
            CancelAbility(Manager, Value, InputAction);
        }
    }
    else
    {
        const UEnum* InputActionValueTypeEnum = StaticEnum<EInputActionValueType>();
        const int32 EnumValue = static_cast<uint8>(Value.GetValueType());
        
        UE_LOG(LogNinjaInputHandler, Warning, TEXT("[%s] Invalid Value Type %s for Action %s. It should be a Boolean!"),
            *GetNameSafe(Manager->GetOwner()),
            *InputActionValueTypeEnum->GetDisplayNameTextByIndex(EnumValue).ToString(),
            *GetNameSafe(InputAction));
    }
}

bool UInputHandler_AbilityActivation::TryHandleActiveAbility(UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    if (HasActiveAbility(Manager))
    {
        if (bSendEventIfActive)
        {
            // ReSharper disable once CppExpressionWithoutSideEffects
            SendGameplayEvent(Manager, ActiveEventTag, Value, InputAction);
        }
        if (bToggledActivation)
        {
            CancelAbility(Manager, Value, InputAction);   
        }

        return true;
    }

    return false;
}

void UInputHandler_AbilityActivation::HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    CancelAbility(Manager, Value, InputAction);
}

void UInputHandler_AbilityActivation::HandleCancelledEvent_Implementation(UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    CancelAbility(Manager, Value, InputAction);
}

bool UInputHandler_AbilityActivation::HasActiveAbility(UNinjaInputManagerComponent* Manager) const
{
    // Child classes must implement this function adequately, based on their activation mode.
    checkNoEntry();
    return false;
}

void UInputHandler_AbilityActivation::ActivateAbility(UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    // Child classes must implement this function adequately, based on their activation mode.
    checkNoEntry();
}

void UInputHandler_AbilityActivation::CancelAbility(UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    // Child classes must implement this function adequately, based on their activation mode.
    checkNoEntry();
}
