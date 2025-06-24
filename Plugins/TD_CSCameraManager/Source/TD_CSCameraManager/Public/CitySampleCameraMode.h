// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "CineCameraComponent.h"
#include "CoreMinimal.h"
#include "CitySampleInterpolators.h"

#include "CitySampleCameraMode.generated.h"

class ACitySamplePlayerCameraManager;
class UCameraShakeBase;

// USTRUCT(BlueprintType)
// struct TD_CSCAMERAMANAGER_API FTD_CineCameraSetting
// {
// 	GENERATED_BODY()
// 	
// 	/** 如果为 true，则默认 cine cam 胶片背板设置将被 CineCam_FilmBackOverride 中设置的设置覆盖 */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	bool bOverrideFilmBack = false;
//
// 	/** 启用 OverrideFilmBack 时要使用的自定义胶片背设置 */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bOverrideFilmback))
// 	FCameraFilmbackSettings FilmBackOverride;
// 	
// 	/** 启用 UseCineCamSettings 时使用的摄像机焦距 */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float CurrentFocalLength = 30.f;
//
// 	/** 启用 UseCineCamSettings 时要使用的摄像机光圈 */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float CurrentAperture = 11.f;
//
// 	/** 启用 UseCineCamSettings 时要使用的自定义焦距调整 */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float FocusDistanceAdjustment = 0.f;
//
// 	/** 启用 UseCineCam 时使用的 FOV */
// 	UPROPERTY(VisibleAnywhere)
// 	float DisplayOnly_FOV = 70.f;
// };

/**
 * Camera Mode 实现的基类。具有用于摄像机激活、更新以及是否使用 cine cam 属性的功能和设置
 */
UCLASS(Blueprintable)
class TD_CSCAMERAMANAGER_API UCitySampleCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UCitySampleCameraMode();

	/** 跳过当前相机模式的插值步骤，以根据相机模式干净地处理特定情况 */
	virtual void SkipNextInterpolation();

	/** 通知摄像机它“激活”，即主摄像机。 */
	virtual void OnBecomeActive(AActor* ViewTarget, UCitySampleCameraMode* PreviouslyActiveMode);

	/** 通知摄像机它不再是主摄像机。在混合时可能仍在处理。 */
	virtual void OnBecomeInactive(AActor* ViewTarget, UCitySampleCameraMode* NewActiveMode);

	/** 由 UpdateViewTarget 中的 CameraManager 调用，相机模式实现可以根据需要使用它来定制其更新逻辑 */
	virtual void UpdateCamera(class AActor* ViewTarget, UCineCameraComponent* CineCamComp, float DeltaTime, struct FTViewTarget& OutVT);

	/** 当此模式完全混合并从堆栈中删除时调用，因为它不再有影响 */
	virtual void OnRemovedFromStack() {};

	/** 返回此摄像机模式的摄像机过渡时间 */
	float GetTransitionTime() const;

	/** 此模式绑定到的 Camera actor */
	UPROPERTY(transient)
	ACitySamplePlayerCameraManager* PlayerCamera;

	/** 进入此相机模式时，相机过渡应花费的时间 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition", meta = (DisplayName = "Transition In Time"))
	float TransitionInTime;

	/** 此摄像机模式类型的过渡参数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	FViewTargetTransitionParams TransitionParams;

	/** 所需的 FOV。全角，单位为度（例如 90.f） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings", meta = (EditCondition = "!bUseCineCamSettings"))
	float FOV = 75.f;

	/** 如果为 true，则此摄像机模式将使用电影摄像机组件，从而允许访问电影摄像机设置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam")
	bool bUseCineCam;

	/** 如果为 true，则启用 Use Cine cam 时将应用自定义电影摄像机设置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam")
	bool bUseCineCamSettings;

	/** 如果为 true，则默认 cine cam 胶片背板设置将被 CineCam_FilmBackOverride 中设置的设置覆盖 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam", meta = (EditCondition = bUseCineCamSettings))
	bool bOverrideFilmBack = false;

	/** 启用 OverrideFilmBack 时要使用的自定义胶片背设置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam", meta = (EditCondition = bOverrideFilmback))
	FCameraFilmbackSettings CineCam_FilmBackOverride;
	
	/** 启用 UseCineCamSettings 时使用的摄像机焦距 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam", meta = (EditCondition = bUseCineCamSettings))
	float CineCam_CurrentFocalLength = 30.f;

	/** 启用 UseCineCamSettings 时要使用的摄像机光圈 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam", meta = (EditCondition = bUseCineCamSettings))
	float CineCam_CurrentAperture = 11.f;

	/** 启用 UseCineCamSettings 时要使用的自定义焦距调整 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam", meta = (EditCondition = bUseCineCamSettings))
	float CineCam_FocusDistanceAdjustment = 0.f;

	/** 启用 UseCineCam 时使用的 FOV */
	UPROPERTY(VisibleAnywhere, Category = "CineCam", meta = (EditCondition = bUseCineCamSettings))
	float CineCam_DisplayOnly_FOV = 70.f;

	/** 用于跟踪阻挡摄像机的 actor 的列表 */
	UPROPERTY(Transient)
	TArray<AActor*> BlockingActors;

	/** 蓝图原生事件，用于摄像机模式根据需要重置为默认设置 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetToDefaultSettings();

	/** 
	 * 如果此模式在过渡时应停止模拟，并在活动时混合保持其最后一个 POV，则返回 true
	 * 返回 false 以在过渡时保持模拟
	 */
	virtual bool ShouldLockOutgoingPOV() const;

protected:

	/** 用于跟踪当前相机模式是否处于活动状态的标志 */
	bool bIsActive = false;

	/** 当此相机模式变为活动状态时创建的 Camera Shake 类 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	/** 激活此摄像机模式并设置有效的 CameraShakeClass 时创建的 Camera Shake 对象实例 */
	UPROPERTY(transient)
	UCameraShakeBase* CameraShakeInstance = nullptr;

	/** 如果为 true，则将使用 ShakeScaling_SpeedRange 和 ShakeScaling_ScaleRange 缩放摄像机抖动 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
	bool bScaleShakeWithViewTargetVelocity = false;

	/** 将映射到摄像机抖动刻度的目标速度的视图范围 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake", meta = (EditCondition = bScaleShakeWithViewTargetVelocity))
	FVector2D ShakeScaling_SpeedRange { 0.f, 300.f };

	/** 映射到 ShakeScaling_SpeedRange，因此在 SpeedRange.X 处，shake 将缩放为 ScaleRange.X */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake", meta = (EditCondition = bScaleShakeWithViewTargetVelocity))
	FVector2D ShakeScaling_ScaleRange { 1.f, 3.f };

	/** 用于平滑更改摄像机抖动强度的插值器 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake", meta = (EditCondition = bScaleShakeWithViewTargetVelocity))
	FIIRInterpolatorFloat ShakeScaleInterpolator = FIIRInterpolatorFloat(5.f);

	/** 如果为 true，则相机模式将尝试显示与相机抖动相关的调试信息 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
	bool bDrawDebugShake = false;

	/** 当 UseCustomFocusDistance 为 true 时，应实现此事件以确定适合相机模式的焦距 */
	UFUNCTION(BlueprintImplementableEvent)
	float GetCustomFocusDistance(AActor* ViewTarget, const FTransform& ViewToWorld) const;

	/** 如果为 true，则将通过 GetCustomFocusDistance 蓝图 Implementable Event 的实现为电影摄像机提供自定义焦距 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CineCam", meta = (EditCondition = bUseCineCamSettings))
	bool bUseCustomFocusDistance;

	/** 如果为 true，则自定义视图间距限制将用于此摄像机模式 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Manager Overrides")
	bool bOverrideViewPitchMinAndMax = false;

	/** 当 OverrideViewPitchMinAndMax 为 true 时使用的间距最小限制 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Manager Overrides", meta = (EditCondition = bOverrideViewPitchMinAndMax))
	float ViewPitchMinOverride = 0.f;

	/** 当 OverrideViewPitchMinAndMax 为 true 时使用的 Pitch maximum limit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Manager Overrides", meta = (EditCondition = bOverrideViewPitchMinAndMax))
	float ViewPitchMaxOverride = 0.f;

	/** 尝试强制转换并返回摄像机模式所属的 CitySample Player Controller */
	UFUNCTION(BlueprintCallable, Category = "Camera Mode")
	APlayerController* GetOwningPlayerController() const;
	// template<class T>
	// T* TryGetOwningPlayerController() const
	// {
	// 	return PlayerCamera ? Cast<T>(PlayerCamera->PCOwner) : nullptr;
	// }

	/** 将电影摄像机设置应用于当前摄像机模式（如果相关） */
	void ApplyCineCamSettings(FTViewTarget& OutVT, UCineCameraComponent* CineCamComp, float DeltaTime);

	/** 尝试确定相机模式的所需焦距，无论是正常计算还是使用自定义焦距方法  */
	virtual float GetDesiredFocusDistance(AActor* ViewTarget, const FTransform& ViewToWorld) const;

	/** 如果为 true，则 Camera 模式将重置某些插值器。适用于硬剪辑或独特的摄像机情况 */
	bool bSkipNextInterpolation = false;

	/** 摄像机到世界变换，该变换已缓存，当有人想要轻松访问摄像机的变换时可以使用它 */
	FTransform LastCameraToWorld;
};
