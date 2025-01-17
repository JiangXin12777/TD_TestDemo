// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "NinjaInputSettings.h"

#include "NinjaInputTags.h"

UNinjaInputSettings::UNinjaInputSettings()
{
    TrackedEvents = {
        ETriggerEvent::Started,
        ETriggerEvent::Triggered,
        ETriggerEvent::Ongoing,
        ETriggerEvent::Completed,
        ETriggerEvent::Canceled,
    };
    
	BlockCameraTag = Tag_Input_Block_Camera;
	BlockMovementTag = Tag_Input_Block_Movement;
	BlockRotationTag = Tag_Input_Block_Rotation;
    
	GamepadInputModeTag = Tag_Input_Mode_Gamepad;
	KeyboardAndMouseInputModeTag = Tag_Input_Mode_KeyboardAndMouse;
    
    bMatchHandlersWithContext = true;
    bShowScreenDebugMessages = false;
    DebugMessageDuration = 5.f;
    DebugMessageColor = FColor::Emerald;
}
