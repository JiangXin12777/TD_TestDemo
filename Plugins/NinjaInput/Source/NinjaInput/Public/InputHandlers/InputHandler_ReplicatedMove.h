// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InputHandlers/InputHandler_Move.h"
#include "InputHandler_ReplicatedMove.generated.h"

/**
 * Handles character movement, through the Replicated Movement Input interface.
 */
UCLASS(meta = (DisplayName = "Character: Move (Replicated)"))
class NINJAINPUT_API UInputHandler_ReplicatedMovement : public UInputHandler_Move
{
	
	GENERATED_BODY()

protected:

	virtual void Move_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const override;
	
};
