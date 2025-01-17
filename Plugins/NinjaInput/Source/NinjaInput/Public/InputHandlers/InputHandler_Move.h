// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NinjaInputHandler.h"
#include "InputHandler_Move.generated.h"

/**
 * Handles character movement.
 */
UCLASS(meta = (DisplayName = "Character: Move"))
class NINJAINPUT_API UInputHandler_Move : public UNinjaInputHandler
{
	
	GENERATED_BODY()

public:

	UInputHandler_Move();

protected:

	/** If any of these tags are present, movement is blocked. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	FGameplayTagContainer BlockMovementTags;
	
	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;

	virtual void HandleOngoingEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;

	/**
	 * Checks if the character is allowed to move.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler|Move")	
	bool CanMove(UNinjaInputManagerComponent* Manager) const;
	
	/**
	 * Effective implementation for moving a character.
	 *
	 * @param Manager		Manager component providing access to the affected character.
	 * @param Value			Event magnitude applied to the movement.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler|Move")
	void Move(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const;
	
};
