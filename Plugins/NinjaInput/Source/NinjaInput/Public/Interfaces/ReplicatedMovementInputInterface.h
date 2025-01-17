// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ReplicatedMovementInputInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UReplicatedMovementInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Allows movement to be routed to a replication method, before being sent to the default Pawn Input.
 */
class NINJAINPUT_API IReplicatedMovementInputInterface
{
	
	GENERATED_BODY()

public:

	/**
	 * Analogous to "AddMovementInput, but handles input replication for forward input.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Replicated Movement Input Interface")
	void AddReplicatedForwardMovementInput(FVector WorldDirection, float ScaleValue, bool bForce);

	/**
	 * Analogous to "AddMovementInput, but handles input replication for right input.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Replicated Movement Input Interface")
	void AddReplicatedRightMovementInput(FVector WorldDirection, float ScaleValue, bool bForce);
	
};