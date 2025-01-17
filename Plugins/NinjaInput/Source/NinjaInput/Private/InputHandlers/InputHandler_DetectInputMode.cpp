// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_DetectInputMode.h"

#include "InputAction.h"
#include "NinjaInputManagerComponent.h"
#include "NinjaInputSettings.h"
#include "NinjaInputTags.h"
#include "GameFramework/Controller.h"
#include "Interfaces/InputModeAwareInterface.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

UInputHandler_DetectInputMode::UInputHandler_DetectInputMode()
{
	TriggerEvents.Add(ETriggerEvent::Triggered);

	const UNinjaInputSettings* Settings = GetDefault<UNinjaInputSettings>();
	const FGameplayTag GamepadTag = Settings->GamepadInputModeTag;
	const FGameplayTag KeyboardAndMouseTag = Settings->KeyboardAndMouseInputModeTag;
	
	LoadInputModeMapping(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Detection/IA_Gamepad.IA_Gamepad'"), GamepadTag);
	LoadInputModeMapping(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Detection/IA_Gamepad_Axis.IA_Gamepad_Axis'"), GamepadTag);
	LoadInputModeMapping(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Detection/IA_KeyboardAndMouse.IA_KeyboardAndMouse'"), KeyboardAndMouseTag);
	LoadInputModeMapping(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Detection/IA_KeyboardAndMouse_Axis.IA_KeyboardAndMouse_Axis'"), KeyboardAndMouseTag);
}

bool UInputHandler_DetectInputMode::CanHandle_Implementation(const ETriggerEvent& TriggerEvent,
	const UInputAction* InputAction) const
{
	return Super::CanHandle_Implementation(TriggerEvent, InputAction)
		&& InputModeMappings.Contains(InputAction);
}

void UInputHandler_DetectInputMode::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
	if (Value.GetMagnitude() != 0.f)
	{
		const FGameplayTag InputMode = *InputModeMappings.Find(InputAction);

		APawn* OwningPawn = Manager->GetPawn();
		if (WantsInputMode(OwningPawn, InputMode))
		{
			IInputModeAwareInterface::Execute_SetPlayerInputMode(OwningPawn, InputMode);
		}

		AController* OwningController = Manager->GetController();
		if (WantsInputMode(OwningController, InputMode))
		{
			IInputModeAwareInterface::Execute_SetPlayerInputMode(OwningController, InputMode);
		}
	}
}

void UInputHandler_DetectInputMode::LoadInputModeMapping(const FName& AssetLocation, const FGameplayTag& InputMode)
{
	ConstructorHelpers::FObjectFinder<UInputAction> InputActionRef(*AssetLocation.ToString());
	if (InputActionRef.Succeeded())
	{
		const TObjectPtr<UInputAction> InputAction = InputActionRef.Object;
		InputActions.AddUnique(InputAction);
		InputModeMappings.Add(InputAction, InputMode);
	}
}

bool UInputHandler_DetectInputMode::WantsInputMode(const AActor* Target, const FGameplayTag& InputMode)
{
	return IsValid(Target) && Target->Implements<UInputModeAwareInterface>()
		&& IInputModeAwareInterface::Execute_GetPlayerInputMode(Target) != InputMode;
}