// Copyright 2022 Namman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACMComponent.generated.h"

// Add this component to your character and set "CameraManagerClass" to MainCameraManager
UCLASS(ClassGroup = (CameraManager), DisplayName = "ACMComponent", meta = (BlueprintSpawnableComponent))
class ACTIONCAMERAMANAGER_API UACMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UACMComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
//~=============================================================================
// Init View Target
private:
	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerControllComp;

	UPROPERTY()
	TWeakObjectPtr<class AACMMainCamera> CameraManager;

	UFUNCTION()
	void TryToSetViewTarget();

	FTimerHandle InitTargetTimer;
	

//~=============================================================================
// Switch View Target
public:
	/**
	* Replace the main viewport and the main viewport for the character
	* 
	* @params   BlendTime   Time taken to switch viewport
	*/
	UFUNCTION(BlueprintCallable, Category = "ACM")
	void SwitchViewTarget(float BlendTime = 0.f);

	/**
	* Returns whether the current viewport is the main camera(ACMMainCamera)
	* 
	* @return   whether the current viewport is the main camera(ACMMainCamera)
	*/
	UFUNCTION(BlueprintCallable, Category = "ACM")
	bool CheckViewportIsMain();

private:
	UFUNCTION()
	void SwitchComplete();

	FTimerHandle RotationSwitchHandler;

	uint8 bIsSwitching : 1;
};
