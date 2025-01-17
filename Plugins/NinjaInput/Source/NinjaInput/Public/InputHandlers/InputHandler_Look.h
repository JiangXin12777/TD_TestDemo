// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "NinjaInputHandler.h"
#include "InputHandler_Look.generated.h"

/**
 * Translates mouse or gamepad commands into camera input.
 */
UCLASS(meta = (DisplayName = "Character: Look"))
class NINJAINPUT_API UInputHandler_Look : public UNinjaInputHandler
{
	
	GENERATED_BODY()

public:

	UInputHandler_Look();

protected:

	/** If any of these tags are present, camera movement is blocked. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Look")
	FGameplayTagContainer BlockCameraTags;
	
	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;

	virtual void HandleOngoingEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;

	/**
	 * Checks if the character is allowed to look around.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler|Look")	
	bool CanLook(UNinjaInputManagerComponent* Manager) const;
	
	/**
	 * Effective implementation the camera movement (look).
	 *
	 * @param Manager		Manager component providing access to the affected character.
	 * @param Value			Event magnitude applied to the camera.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler|Look")
	void Look(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const;
	
};
