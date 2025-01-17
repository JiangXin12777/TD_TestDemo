// Copyright 2022 Namman. All Rights Reserved.

#include "ACMComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "ACMMainCamera.h"
#include "TimerManager.h"
#include "Engine/World.h"

UACMComponent::UACMComponent()
	:bIsSwitching(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UACMComponent::BeginPlay()
{
	Super::BeginPlay();

	// Find MainCameraManager & Set
	AACMMainCamera* TargetCamera = Cast<AACMMainCamera>(UGameplayStatics::GetActorOfClass(this, AACMMainCamera::StaticClass()));
	if (TargetCamera)
	{
		CameraManager = TargetCamera;
		CameraManager->SetPlayer(GetOwner());

		// Move the Player Controller to MainCameraManager (By NetworkMode) 
		ACharacter* Player = Cast<ACharacter>(GetOwner());
		PlayerControllComp = Cast<APlayerController>(Player->GetController());

		ENetMode NetMode = GetNetMode();
		if (PlayerControllComp.IsValid() && (NetMode != NM_DedicatedServer || (NetMode == NM_DedicatedServer && !GetOwner()->HasAuthority())))
		{
			GetWorld()->GetTimerManager().SetTimer(InitTargetTimer, this, &UACMComponent::TryToSetViewTarget, 0.1f, true);
		}
	}
}
void UACMComponent::TryToSetViewTarget()
{
	if (PlayerControllComp.IsValid() && CameraManager.IsValid())
	{
		PlayerControllComp->bAutoManageActiveCameraTarget = false;
		PlayerControllComp->SetViewTarget(CameraManager.Get());
		GetWorld()->GetTimerManager().ClearTimer(InitTargetTimer);
	}
}
void UACMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Setting the rotation of the controller according to the rotation of the CameraManager 
	if (!bIsSwitching && CameraManager.IsValid() && PlayerControllComp.IsValid() && CheckViewportIsMain())
	{
		FRotator CameraRot = FRotator(CameraManager->GetActorRotation().Pitch, CameraManager->GetActorRotation().Yaw, PlayerControllComp->GetControlRotation().Roll);

		ENetMode NetMode = GetNetMode();
		if (NetMode != NM_DedicatedServer || (NetMode == NM_DedicatedServer && !GetOwner()->HasAuthority()))
		{
			PlayerControllComp->ClientSetRotation(CameraRot);
		}
	}
}
void UACMComponent::SwitchViewTarget(float BlendTime)
{
	if (!PlayerControllComp.IsValid() || !CameraManager.IsValid())
	{
 		UE_LOG(LogTemp, Error,TEXT("Switch Target is Error!"));
		return;
	}

	// Check Current View State
	AActor* TargetActor;	
	if(!CheckViewportIsMain())
	{
		TargetActor = CameraManager.Get();

		GetWorld()->GetTimerManager().ClearTimer(RotationSwitchHandler);
		GetWorld()->GetTimerManager().SetTimer(RotationSwitchHandler, this, &UACMComponent::SwitchComplete, BlendTime, false);
	}
	else
	{
		TargetActor = GetOwner();
	}

	// Set New Target 
	bIsSwitching = true;
	PlayerControllComp->SetViewTargetWithBlend(TargetActor, BlendTime);
}
bool UACMComponent::CheckViewportIsMain()
{
	if (PlayerControllComp.IsValid())
	{
		return (PlayerControllComp.Get()->PlayerCameraManager->ViewTarget.Target != GetOwner()) ? true : false;
	}

	return false;
}
void UACMComponent::SwitchComplete()
{
	bIsSwitching = false;
}