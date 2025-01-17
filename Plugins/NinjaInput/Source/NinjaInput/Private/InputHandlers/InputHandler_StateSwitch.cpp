// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_StateSwitch.h"

#include "NinjaInputManagerComponent.h"
#include "Interfaces/InputStateSwitchInterface.h"

UInputHandler_StateSwitch::UInputHandler_StateSwitch()
{
    StateSwitchTag = FGameplayTag::EmptyTag;
    TriggerEvents.Add(ETriggerEvent::Triggered);
}

void UInputHandler_StateSwitch::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    const bool bNewValue = !IsActive(Manager); 
    SetSwitchState(Manager, bNewValue);
}

bool UInputHandler_StateSwitch::IsActive(const UNinjaInputManagerComponent* Manager) const
{
    check(Manager);
    ensure(StateSwitchTag.IsValid());

    const TObjectPtr<AActor> Owner = Manager->GetOwner();
    if (ensure(IsValid(Owner) && Owner->Implements<UInputStateSwitchInterface>()))
    {
        return IInputStateSwitchInterface::Execute_GetSwitchState(Owner, StateSwitchTag);
    }

    return false;
}

void UInputHandler_StateSwitch::SetSwitchState(const UNinjaInputManagerComponent* Manager, const bool NewState) const
{
    check(Manager);
    ensure(StateSwitchTag.IsValid());
    
    const TObjectPtr<AActor> Owner = Manager->GetOwner();
    if (ensure(IsValid(Owner) && Owner->Implements<UInputStateSwitchInterface>()))
    {
        return IInputStateSwitchInterface::Execute_SetSwitchState(Owner, StateSwitchTag, NewState);
    }
}
