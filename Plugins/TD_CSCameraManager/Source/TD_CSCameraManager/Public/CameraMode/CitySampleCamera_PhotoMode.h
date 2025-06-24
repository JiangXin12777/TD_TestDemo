// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CitySampleCameraMode.h"
#include "CitySampleCamera_PhotoMode.generated.h"

/**
 * Camera mode that works in tandem with the PhotoModeComponent to apply user controlled adjustments to the cine cam component used for Photo Mode
 */
UCLASS(Blueprintable)
class TD_CSCAMERAMANAGER_API UCitySampleCamera_PhotoMode : public UCitySampleCameraMode
{
	GENERATED_BODY()

public:

	UCitySampleCamera_PhotoMode();

	
	//~ Begin UCitySampleCameraMode Interface
	virtual void UpdateCamera(class AActor* ViewTarget, UCineCameraComponent* CineCamComp, float DeltaTime, struct FTViewTarget& OutVT) override;
	virtual void OnBecomeActive(AActor* ViewTarget, UCitySampleCameraMode* PreviouslyActiveMode) override;
	//~ End UCitySampleCameraMode Interface

	
	UFUNCTION(BlueprintPure)
	float GetPhotoModeCustomFocusDistance() const { return CustomFocusDistance;}

protected:

	/** 距离 用于自动对焦跟踪，也是焦距的默认最大值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhotoModeAutoFocusTraceDistance = 5000.0f;
	 
	/** 用于自动对焦跟踪的球体半径。这在尝试聚焦于世界中的物体时提供了一点回旋余地 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SphereTraceRadius = 12.0f;

private:
	
	/** 返回 PlayerController 的当前 PhotoMode 组件 */
	class UPhotoModeComponent* GetPhotoModeComponent() const;
	
	/** 确保正确应用手动或自动 曝光值 */
	void UpdateExposureSettings(const struct FPhotoModeSettings& Settings, struct FTViewTarget& OutVT) const;
	
	/** 确保正确应用手动或自动 对焦 距离 */
	void UpdateFocusSettings(UPhotoModeComponent* PhotoModeComponent, const struct FPhotoModeSettings& Settings, UCineCameraComponent* CineCamComp, const struct FTViewTarget& VT);

	/** 确保此摄像机模式的视图目标的旋转与 Photo Mode pawn 的视图旋转相匹配 */
	void UpdateFinalRotation(const AActor* ViewTarget, const struct FPhotoModeSettings& Settings, struct FTViewTarget& OutVT);

	/** 接收 Trace 数据并确定要使用的 AsyncFocusDistance */
	void HandleAsyncFocusTrace(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum);

	/** 存储由异步跟踪确定的焦距 */
	float AsyncFocusDistance;

	/** 存储用户设置的手动焦距 */
	float CustomFocusDistance;

	/** 存储由当前后期处理体积确定的基本曝光偏差值 */
	float BaseExposureBias = 1;

	/** 尝试确定自动对焦距离时用于异步跟踪的 Trace 委托 */
	FTraceDelegate AsyncFocusTraceDelegate;
};
