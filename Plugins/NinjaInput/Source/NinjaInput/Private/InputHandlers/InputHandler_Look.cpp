// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_Look.h"

#include "InputAction.h"
#include "NinjaInputManagerComponent.h"
#include "NinjaInputSettings.h"
#include "NinjaInputTags.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

UInputHandler_Look::UInputHandler_Look()
{
	BlockCameraTags = FGameplayTagContainer::EmptyContainer;
	BlockCameraTags.AddTagFast(GetDefault<UNinjaInputSettings>()->BlockCameraTag);
	
	TriggerEvents.Add(ETriggerEvent::Triggered);
	TriggerEvents.Add(ETriggerEvent::Ongoing);

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRef(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Input/IA_Look.IA_Look'"));
	if (InputActionRef.Succeeded())
	{
		const TObjectPtr<UInputAction> InputAction = InputActionRef.Object;
		InputActions.AddUnique(InputAction);
	}
}

void UInputHandler_Look::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	Look(Manager, Value);
}

void UInputHandler_Look::HandleOngoingEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	Look(Manager, Value);
}

bool UInputHandler_Look::CanLook_Implementation(UNinjaInputManagerComponent* Manager) const
{
	// Ensure that we DO NOT have any of the blocking tags.
	return !HasAnyTags(Manager, BlockCameraTags);
}

void UInputHandler_Look::Look_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value) const
{
	if (Value.GetMagnitude() != 0.f && CanLook(Manager))
	{
		APawn* Pawn = Cast<APawn>(Manager->GetOwner());
		if (IsValid(Pawn))
		{ 
			Pawn->AddControllerYawInput(Value[0]);
			Pawn->AddControllerPitchInput(Value[1]);
		}
	}
}
