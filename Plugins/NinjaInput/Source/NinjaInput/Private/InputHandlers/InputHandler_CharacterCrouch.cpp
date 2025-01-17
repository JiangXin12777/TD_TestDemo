// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_CharacterCrouch.h"

#include "InputAction.h"
#include "NinjaInputManagerComponent.h"
#include "NinjaInputSettings.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

UInputHandler_CharacterCrouch::UInputHandler_CharacterCrouch()
{
	BlockCrouchTags = FGameplayTagContainer::EmptyContainer;
	BlockCrouchTags.AddTagFast(GetDefault<UNinjaInputSettings>()->BlockMovementTag);
	
	TriggerEvents.Add(ETriggerEvent::Triggered);

	static ConstructorHelpers::FObjectFinder<UInputAction> CrouchToggleInputActionRef(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Input/IA_Crouch_Toggle.IA_Crouch_Toggle'"));
	if (CrouchToggleInputActionRef.Succeeded())
	{
		const TObjectPtr<UInputAction> InputAction = CrouchToggleInputActionRef.Object;
		InputActions.AddUnique(InputAction);
	}
}

void UInputHandler_CharacterCrouch::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	if (!HasAnyTags(Manager, BlockCrouchTags))
	{
		ACharacter* OwningCharacter = Cast<ACharacter>(Manager->GetPawn());
		if (IsValid(OwningCharacter))
		{
			if (OwningCharacter->bIsCrouched)
			{
				OwningCharacter->UnCrouch();
			}
			else if (OwningCharacter->GetCharacterMovement()->CanEverCrouch())
			{
				OwningCharacter->Crouch();
			}
		}
	}
}
