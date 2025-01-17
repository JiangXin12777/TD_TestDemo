// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "NinjaInputHandler.h"
#include "InputHandler_CharacterJump.generated.h"

/**
 * Replicates the original Jump setup for Character owners.
 *
 * Avoid mixing default Character actions, such as this one, when you are using the Gameplay
 * Ability System as it provides a Jump Gameplay Ability that can be used instead.
 */
UCLASS(meta = (DisplayName = "Character: Jump"))
class NINJAINPUT_API UInputHandler_CharacterJump : public UNinjaInputHandler
{
	
	GENERATED_BODY()

public:

	UInputHandler_CharacterJump();

protected:

	/** Gameplay tags that, if present, will block the ability of jumping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Jump")
	FGameplayTagContainer BlockJumpTags;
	
	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;

	virtual void HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;
	
};
