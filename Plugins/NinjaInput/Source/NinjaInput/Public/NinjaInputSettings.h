// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputTriggers.h"
#include "Engine/DeveloperSettings.h"
#include "NinjaInputSettings.generated.h"

/**
 * Central location for system-wide settings related to the Ninja Input plugin.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Ninja Input"))
class NINJAINPUT_API UNinjaInputSettings : public UDeveloperSettings
{
	
	GENERATED_BODY()

public:

    /** Input Events that are tracked by managers and delegated to registered Handlers. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Core Settings")
    TArray<ETriggerEvent> TrackedEvents;
    
	/** Gameplay Tag that, if present, blocks camera movement. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input Blocking")
	FGameplayTag BlockCameraTag;

	/**
	 * Gameplay Tag that, if present, blocks the character movement.
	 * 
	 * This tag also blocks movement-related actions such as Jump, and changes in
	 * the character's movement mode (ie. Standing to Crouch and vice-versa).
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input Blocking")
	FGameplayTag BlockMovementTag;

	/** Gameplay Tag that, if present, blocks character rotation. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input Blocking")
	FGameplayTag BlockRotationTag;

	/** Gameplay Tag that represents a Gamepad Input mode. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input Modes", DisplayName = "Input Mode: Gamepad")
	FGameplayTag GamepadInputModeTag;

	/** Gameplay Tag that represents a Keyboard and Mouse Input mode. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input Modes", DisplayName = "Input Mode: Keyboard and Mouse")
	FGameplayTag KeyboardAndMouseInputModeTag;

    /**
     * Enables data validation for the Setup Asset.
     * 
     * Highly recommended to keep this enabled to avoid common mistakes in the Setup Data!
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development")
    bool bEnableDataValidation;

    /**
     * If enabled, it will ensure that all Input Actions from the Context have an equivalent Handler.
     *
     * Can only be used if the Data Validation is enabled.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development", meta = (EditCondition = bEnableDataValidation, EditConditionHides))
    bool bMatchHandlersWithContext;
    
    /**
     * Determines if debug messages show be added to screen for all input handlers.
     *
     * This option is only valid in the editor and won't be added to packaged builds.
     */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development")
    bool bShowScreenDebugMessages;

    /** Duration for the Debug Message on screen. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development", meta = (EditCondition = bShowScreenDebugMessages, EditConditionHides))
    float DebugMessageDuration;
    
    /** Color used for the on-screen debug messages. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development", meta = (EditCondition = bShowScreenDebugMessages, EditConditionHides))
    FColor DebugMessageColor;
    
	UNinjaInputSettings();
	
};
