// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_CharacterJump.h"

#include "InputAction.h"
#include "NinjaInputManagerComponent.h"
#include "NinjaInputSettings.h"
#include "GameFramework/Character.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

UInputHandler_CharacterJump::UInputHandler_CharacterJump()
{
	BlockJumpTags = FGameplayTagContainer::EmptyContainer;
	BlockJumpTags.AddTagFast(GetDefault<UNinjaInputSettings>()->BlockMovementTag);
	
	TriggerEvents.Add(ETriggerEvent::Triggered);
	TriggerEvents.Add(ETriggerEvent::Completed);

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRef(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Input/IA_Jump.IA_Jump'"));
	if (InputActionRef.Succeeded())
	{
		const TObjectPtr<UInputAction> InputAction = InputActionRef.Object;
		InputActions.AddUnique(InputAction);
	}
}

void UInputHandler_CharacterJump::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	ACharacter* OwningCharacter = Cast<ACharacter>(Manager->GetPawn());
	if (IsValid(OwningCharacter))
	{
		OwningCharacter->Jump();
	}
}

void UInputHandler_CharacterJump::HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	ACharacter* OwningCharacter = Cast<ACharacter>(Manager->GetPawn());
	if (IsValid(OwningCharacter))
	{
		OwningCharacter->StopJumping();
	}
}