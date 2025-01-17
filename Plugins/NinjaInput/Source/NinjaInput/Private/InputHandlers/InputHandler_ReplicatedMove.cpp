// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "InputHandlers/InputHandler_ReplicatedMove.h"

#include "NinjaInputManagerComponent.h"
#include "Interfaces/ReplicatedMovementInputInterface.h"

void UInputHandler_ReplicatedMovement::Move_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const
{
	if (Value.GetMagnitude() != 0.f && CanMove(Manager))
	{
		AActor* Target = Manager->GetOwner(); 
		if (IsValid(Target) && Target->Implements<UReplicatedMovementInputInterface>())
		{
			const FVector ForwardDirection = Manager->GetForwardVector();
			IReplicatedMovementInputInterface::Execute_AddReplicatedForwardMovementInput(Target, ForwardDirection, Value[1], false);

			const FVector RightDirection = Manager->GetRightVector();
			IReplicatedMovementInputInterface::Execute_AddReplicatedRightMovementInput(Target, RightDirection, Value[0], false);
		}
	}
}
