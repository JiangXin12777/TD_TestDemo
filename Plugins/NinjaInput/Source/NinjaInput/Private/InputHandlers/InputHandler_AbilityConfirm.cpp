// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_AbilityConfirm.h"

#include "AbilitySystemComponent.h"
#include "NinjaInputManagerComponent.h"

UInputHandler_AbilityConfirm::UInputHandler_AbilityConfirm()
{
	TriggerEvents.Add(ETriggerEvent::Triggered);
}

void UInputHandler_AbilityConfirm::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	UAbilitySystemComponent* AbilitySystemComponent = Manager->GetAbilitySystemComponent();
	if (ensure(IsValid(AbilitySystemComponent)))
	{
		AbilitySystemComponent->LocalInputConfirm();
	}
}
