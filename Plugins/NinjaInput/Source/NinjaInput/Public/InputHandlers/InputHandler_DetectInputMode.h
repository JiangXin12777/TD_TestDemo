// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "NinjaInputHandler.h"
#include "InputHandler_DetectInputMode.generated.h"

/**
 * Determines the Input Mode based on the detection mapping/input actions.
 *
 * Once triggered, it will look for the current Pawn and Controller responsible for activating
 * the Input Action and, if they are valid implementations of the Input Mode Aware Interface,
 * then the Input Mode will be provided.
 */
UCLASS(DisplayName = "Utility: Detect Input Mode")
class NINJAINPUT_API UInputHandler_DetectInputMode : public UNinjaInputHandler
{
	
	GENERATED_BODY()

public:

	UInputHandler_DetectInputMode();

	virtual bool CanHandle_Implementation(const ETriggerEvent& TriggerEvent,
		const UInputAction* InputAction) const override;
	
protected:

	/** Maps Input Actions to the related Gameplay Tag. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detect Input Mode")
	TMap<TObjectPtr<UInputAction>, FGameplayTag> InputModeMappings;
	
	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
		const FInputActionValue& Value, const UInputAction* InputAction) const override;
	
private:
	
	void LoadInputModeMapping(const FName& AssetLocation, const FGameplayTag& InputMode);

	static bool WantsInputMode(const AActor* Target, const FGameplayTag& InputMode);
	
};

