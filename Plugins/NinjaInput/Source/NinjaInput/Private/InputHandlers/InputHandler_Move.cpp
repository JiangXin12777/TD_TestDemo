// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_Move.h"

#include "InputAction.h"
#include "NinjaInputManagerComponent.h"
#include "NinjaInputSettings.h"
#include "NinjaInputTags.h"
#include "GameFramework/Character.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

UInputHandler_Move::UInputHandler_Move()
{
	BlockMovementTags = FGameplayTagContainer::EmptyContainer;
	BlockMovementTags.AddTagFast(GetDefault<UNinjaInputSettings>()->BlockMovementTag);
	
	TriggerEvents.Add(ETriggerEvent::Triggered);
	TriggerEvents.Add(ETriggerEvent::Ongoing);

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRef(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Input/IA_Move.IA_Move'"));
	if (InputActionRef.Succeeded())
	{
		const TObjectPtr<UInputAction> InputAction = InputActionRef.Object;
		InputActions.AddUnique(InputAction);
	}
}

void UInputHandler_Move::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	Move(Manager, Value);
}

void UInputHandler_Move::HandleOngoingEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	Move(Manager, Value);
}

bool UInputHandler_Move::CanMove_Implementation(UNinjaInputManagerComponent* Manager) const
{
	// Ensure that we DO NOT have any of the blocking tags.
	return !HasAnyTags(Manager, BlockMovementTags);
}

void UInputHandler_Move::Move_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const
{
	if (Value.GetMagnitude() != 0.f && CanMove(Manager))
	{
		if (APawn* Pawn = Cast<APawn>(Manager->GetOwner()))
		{
			const FVector ForwardDirection = Manager->GetForwardVector();
			Pawn->AddMovementInput(ForwardDirection, Value[1]);
			
			const FVector RightDirection = Manager->GetRightVector();
			Pawn->AddMovementInput(RightDirection, Value[0]);
		}
	}
}
