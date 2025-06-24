// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CitySampleInterpolators.h"

#include "PhotoModeComponent.generated.h"

class ACitySamplePlayerCameraManager;
class APlayerController;
class ADefaultPawn;

DECLARE_LOG_CATEGORY_EXTERN(TD_PhotoModeLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoModeActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotomodeDeactivate);

/**
 * 帮助管理 Photo Mode 状态的 Enum
 */
UENUM(BlueprintType)
enum class EPhotoModeState : uint8
{
	/** 未激活 */
	NotActive,

	/** 待定 */
	Pending,

	/** 激活 */
	Active
};

/**
 * 用于存储所有可调整照片模式参数的结构
 */
USTRUCT(BlueprintType)
struct FPhotoModeSettings
{
	GENERATED_BODY()
public:

	/** 手动曝光偏差 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ManualExposureBias = 0.0f;

	/** 启用自动对焦 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoFocusEnabled = false;

	/** 手动焦距 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ManualFocusDistance = 0.0f;

	/** 焦距调整 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FocusDistanceAdjustment = 0.0f;

	/** 当前焦距 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CurrentFocalLength = 0.0f;

	/** 电流孔径 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CurrentAperture = 0.0f;
};

/**
 * 用于激活、停用和管理照片模式的组件
 */
UCLASS(BlueprintType, Blueprintable)
class TD_CSCAMERAMANAGER_API UPhotoModeComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	UPhotoModeComponent();

	
	//~ Begin ActorComponent Interface
	virtual void OnComponentCreated() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//~ End ActorComponent Interface

	
	UFUNCTION(BlueprintPure)
	EPhotoModeState GetPhotoModeState() const
	{
		return State;
	}
	
	/** 初始化 Photo Mode 并尝试生成和占有 Photo Mode pawn */
	UFUNCTION(BlueprintCallable)
	bool ActivatePhotoMode();
	
	/** 激活照片模式时的 BP 挂钩 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivatePhotoMode();

	/** 停用照片模式并处理到之前游戏状态的正确过渡 */
	UFUNCTION(BlueprintCallable)
	void DeactivatePhotoMode();

	/** 照片模式停用时的 BP 挂钩 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeactivatePhotoMode();

	/** 添加与照片模式相关的输入映射 */
	UFUNCTION(BlueprintCallable)
	void AddPhotoModeInputContext();

	/** 删除与照片模式相关的输入映射 */
	UFUNCTION(BlueprintCallable)
	void RemovePhotoModeInputContext();

	/** 启用自动对焦并初始化自动对焦插值器 */
	UFUNCTION(BlueprintCallable)
	void EnableAutoFocus();

	/** 启用自动对焦时的 BP 钩 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnEnableAutoFocus();

	/** 禁用自动对焦，将焦距保持在禁用时设置的任何值 */
	UFUNCTION(BlueprintCallable)
	void DisableAutoFocus();

	/** 禁用自动对焦时的 BP 钩 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDisableAutoFocus();

	/** 将当前照片模式设置保存为用于重置的新默认设置 */
	UFUNCTION(BlueprintCallable)
	void SaveCurrentSettingsAsDefault();

	UFUNCTION(BlueprintCallable)
	void SetUseAutoFocus(bool bUseAutoFocus)
	{
		Settings.bAutoFocusEnabled = bUseAutoFocus;
	}

	UFUNCTION(BlueprintCallable)
	void SetCurrentFocalLength(float NewFocalLength)
	{
		Settings.CurrentFocalLength = NewFocalLength;
	}

	UFUNCTION(BlueprintCallable)
	void SetCurrentAperture(float NewAperture)
	{
		Settings.CurrentAperture = NewAperture;
	}

	UFUNCTION(BlueprintCallable)
	void SetFocusDistanceAdjustment(float NewFocusDistanceAdjustment)
	{
		Settings.FocusDistanceAdjustment = NewFocusDistanceAdjustment;
	}

	UFUNCTION(BlueprintCallable)
	void SetManualExposureBias(float NewManualExposureBias)
	{
		Settings.ManualExposureBias = NewManualExposureBias;
	}

	UFUNCTION(BlueprintCallable)
	void SetManualFocusDistance(float NewManualFocusDistance)
	{
		Settings.ManualFocusDistance = NewManualFocusDistance;
	}
	
	UFUNCTION(BlueprintPure)
	const FPhotoModeSettings& GetPhotoModeSettings() const
	{
		return Settings;
	}

	UFUNCTION(BlueprintPure)
	APawn* GetCachedPlayerPawn()
	{
		return OldPawn;
	}

	void SetGoalAutoFocusDistance(float NewGoalAutoFocusDistance) 
	{ 
		GoalAutoFocusDistance = NewGoalAutoFocusDistance; 
	}

	/** 由此组件的所有者调用，以将输入法绑定到其相应的输入作 */
	void SetUpInputs();

protected:

	/** 在照片模式下隐藏 UI 时的 BP 钩子 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHideUIToggle(bool bIsHiding);

	/** 激活 Photo Mode 时生成和占有的 Pawn 类 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADefaultPawn> PhotoModePawnClass;

	/** 用于平滑调整自动焦距值的插值器 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIIRInterpolatorFloat AutoFocusDistanceInterpolator = FIIRInterpolatorFloat(8.0f);

	/** 照片模式 pawn 是否已经从其原始位置和方向移动了足够远，以保证淡化过渡 */
	UFUNCTION(BlueprintPure, Category = "FadeOut")
	bool ShouldTriggerFadeOut() const;

	/** Photo Mode 进入 Photo Mode 时的 Pawn 初始位置。 */
	UPROPERTY(Transient, BlueprintReadOnly, Category="FadeOut")
	FVector StartingLocation;

	/** Photo Mode 进入 Photo Mode 时 Pawn 初始旋转。 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "FadeOut")
	FRotator StartingRotation;

	/** Photo Mode 退出 Photo Mode 时 Pawn 的最终位置。 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "FadeOut")
	FVector EndingLocation;

	/** Photo Mode 退出 Photo Mode 时 Pawn 的最终旋转。 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "FadeOut")
	FRotator EndingRotation;

	/** 保证 Fade Out 过渡之前允许的 Translational Difference Threshold */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FadeOut")
	float FadeOutDistanceThreshold = 50.0f;

	/** 在保证 Fade Out 过渡之前允许的 Rotation Difference Threshold */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FadeOut")
	float FadeOutRotationThreshold = 30.0f;	

	/** 请务必了解，因为如果我们之前的 pawn 是 drone，我们不想触发淡入淡出过渡 */
	bool bWasOldPawnADrone = false;

private:

	friend class UCitySampleCamera_PhotoMode;

	/** 照片模式激活的最后一步，从我们的 Camera Mode 实例调用，我们可以确定照片模式已成功激活。 */
	void OnPhotoModeActivated_Internal(class UCitySampleCamera_PhotoMode* ActivatedPhotoMode);

	/** 更新我们的 OldPawn 变量，以便我们尝试在照片模式停用时拥有正确的 pawn */
	// UFUNCTION()
	// void OnVehicleDriverExit(class UDrivableVehicleComponent* const DrivableComponent, class ACitySampleVehicleBase* const Vehicle, APawn* const Driver);

	/** 用于照片模式的相机模式 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCitySampleCamera_PhotoMode> PhotoModeClass;

	/** 最新的照片模式设置 */
	UPROPERTY(EditDefaultsOnly)
	FPhotoModeSettings Settings;

	/** 对激活 PhotoMode 时生成和占有的 pawn 的引用 */
	UPROPERTY(Transient)
	class ADefaultPawn* CameraPawn = nullptr;

	/** 引用了 Photo Mode pawn 移动所依赖的 HoverDroneMovementComponent */
	// UPROPERTY(Transient)
	// class UHoverDroneMovementComponent* HoverDroneMovementComp = nullptr;

	/** 引用了在进入照片模式之前拥有的 pawn */
	UPROPERTY(Transient)
	class APawn* OldPawn = nullptr;

	/** 对请求激活照片模式的播放器的引用 */
	UPROPERTY(Transient)
	APlayerController* RequestingPlayer = nullptr;

	UPROPERTY(Transient)
	TWeakObjectPtr<ACitySamplePlayerCameraManager> TD_PlayerCameraManager;

	/** 缓存的默认照片模式重置设置 */
	UPROPERTY(Transient)
	FPhotoModeSettings DefaultSettings;

	/** 当前照片模式状态，跟踪我们是处于非活动状态、活动状态还是介于两者之间 */
	EPhotoModeState State = EPhotoModeState::NotActive;

	/** 我们在使用 Auto Focus 时插值的 Auto focus 距离 */
	float GoalAutoFocusDistance;

	/** 照片模式所依赖的输入映射上下文和输入作 */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* ActiveInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ChangeAltitudeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* UseAutoFocusAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 InputMappingPriority = 1;

	/** 对应于使用左摇杆时的移动速度 */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float HorizontalMovementRate = 1.0f;

	/** 对应于使用触发器输入上升/下降时的移动速率 */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float VerticalMovementRate = 1.0f;

	/** 对应于摄像机垂直查看速率 */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float LookUpRate = 1.0f;

	/** 对应于摄像机水平查看速率 */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float TurnRate = 1.0f;

	/** 用于 pawn 移动函数的包装器，用于在继续之前检查有效的 CameraPawn 引用 */
	void MoveForward(float Val);

	void MoveRight(float Val);

	void MoveUp(float Val);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);
	
	/** 绑定到输入作以促进照片模式特定输入的方法 */
	void MoveActionBinding(const struct FInputActionValue& ActionValue);
	void LookActionBinding(const struct FInputActionValue& ActionValue);
	void ChangeAltitudeActionBinding(const struct FInputActionValue& ActionValue);
	void EnableAutoFocusActionBinding();
	void DisableAutoFocusActionBinding();
};