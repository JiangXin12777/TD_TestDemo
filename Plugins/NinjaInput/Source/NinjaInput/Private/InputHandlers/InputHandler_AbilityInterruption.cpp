// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_AbilityInterruption.h"

#include "NinjaInputManagerComponent.h"

UInputHandler_AbilityInterruption::UInputHandler_AbilityInterruption()
{
    TriggerEvents.Add(ETriggerEvent::Triggered);
}
