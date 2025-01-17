// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_AbilityCancel.h"

#include "AbilitySystemComponent.h"
#include "NinjaInputManagerComponent.h"

UInputHandler_AbilityCancel::UInputHandler_AbilityCancel()
{
	TriggerEvents.Add(ETriggerEvent::Triggered);
}

void UInputHandler_AbilityCancel::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	UAbilitySystemComponent* AbilitySystemComponent = Manager->GetAbilitySystemComponent();
	if (ensure(IsValid(AbilitySystemComponent)))
	{
		AbilitySystemComponent->LocalInputCancel();
	}
}
