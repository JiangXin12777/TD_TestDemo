// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_HUDEvent.h"

#include "NinjaInputManagerComponent.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/HUDEventDispatcherInterface.h"

UInputHandler_HUDEvent::UInputHandler_HUDEvent()
{
	EventTag = FGameplayTag::EmptyTag;
	TriggerEvents.Add(ETriggerEvent::Triggered);
}

void UInputHandler_HUDEvent::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	if (ensure(EventTag.IsValid()))
	{
		const APlayerController* PlayerController = Cast<APlayerController>(Manager->GetController());
		if (IsValid(PlayerController))
		{
			AHUD* HUD = PlayerController->GetHUD();
			if (IsValid(HUD) && HUD->Implements<UHUDEventDispatcherInterface>())
			{
				IHUDEventDispatcherInterface::Execute_HandleInputEvent(HUD, EventTag);
			}
		}
	}
}
