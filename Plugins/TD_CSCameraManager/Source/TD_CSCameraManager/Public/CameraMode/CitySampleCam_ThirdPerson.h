// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CitySampleCameraMode.h"
#include "CitySampleInterpolators.h"

#include "CitySampleCam_ThirdPerson.generated.h"

class UCurveVector;

/**
 * 此相机模式支持的自动跟随类型
 */
UENUM(BlueprintType)
enum class ECameraAutoFollowMode : uint8
{
	None,
	LazyFollow,
	FullFollow
};

/**
 * 定义用于避免摄像机穿透的塞尺的结构体。
 */
USTRUCT(BlueprintType)
struct FPenetrationAvoidanceRay
{
	GENERATED_BODY()

	/** FRotator 描述与主光线的偏差 */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceRay)
	FRotator AdjustmentRot;

	/** 如果这个触角击中世界，它对最终位置的影响有多大 */
	UPROPERTY(EditAnywhere, Category= PenetrationAvoidanceRay)
	float WorldWeight = 0.f;

	/** 在跟踪此 Feeler 时用于碰撞的 extent */
	UPROPERTY(EditAnywhere, Category= PenetrationAvoidanceRay)
	float Radius = 0.f;

	/** 如果上一帧未命中任何内容，则使用此 Feeler 的跟踪之间的最小帧间隔 */
	UPROPERTY(EditAnywhere, Category= PenetrationAvoidanceRay)
	int32 TraceInterval = 0;

	/** 自使用此 Feeler 以来的帧数 */
	UPROPERTY(transient)
	int32 FramesUntilNextTrace = 0;

	UPROPERTY(EditAnywhere, Category = PenetrationAvoidanceRay)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category = PenetrationAvoidanceRay)
	bool bPrimaryRay = false;

	FPenetrationAvoidanceRay()
		: AdjustmentRot(ForceInit)
	{
	}

	FPenetrationAvoidanceRay(FRotator InAdjustmentRot, 
							 float InWorldWeight, 
							 float InRadius, 
							 int32 InTraceInterval,
							 bool bInPrimaryRay)
		: AdjustmentRot(InAdjustmentRot)
		, WorldWeight(InWorldWeight)
		, Radius(InRadius)
		, TraceInterval(InTraceInterval)
		, bPrimaryRay(bInPrimaryRay)
	{}
};

/**
 * CitySampleCameraMode 的第三人称相机实现。
 * 具有相机平滑、自动跟随行为和渗透避免功能。
 */
UCLASS(Blueprintable)
class TD_CSCAMERAMANAGER_API UCitySampleCam_ThirdPerson : public UCitySampleCameraMode
{
	GENERATED_BODY()
	
public:
	UCitySampleCam_ThirdPerson();

	
	//~ Begin UCitySampleCameraMode Interface
	virtual void UpdateCamera(class AActor* ViewTarget, UCineCameraComponent* CineCamComp, float DeltaTime, struct FTViewTarget& OutVT) override;
	virtual void OnBecomeActive(AActor* ViewTarget, UCitySampleCameraMode* PreviouslyActiveMode) override;
	virtual void SkipNextInterpolation() override;
	//~ End UCitySampleCameraModeInterface

	
protected:

	/** Pivot 的 Transform，在 ViewTarget 的空间中。这是摄像机旋转的点。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FTransform PivotToViewTarget;

	/** 用于平滑更改摄像机枢轴在世界空间中的位置的插值器。注： 对于非常快速移动的对象，您可能希望将此设置为 0,0 以进行即时枢轴更新 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FDoubleIIRInterpolatorVector PivotLocInterpolator = FDoubleIIRInterpolatorVector(4.f, 12.f);

	/** 用于在世界空间中平滑更改摄像机枢轴旋转的插值器 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FDoubleIIRInterpolatorRotator PivotRotInterpolator = FDoubleIIRInterpolatorRotator(4.f, 7.f);

	/** 摄像机枢轴的最小和最大俯仰阈值（以度为单位） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FVector2D PivotPitchLimits;

	/** 相机枢轴的最小和最大偏转阈值（以度为单位） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FVector2D PivotYawLimits;

	/** 枢轴空间中摄像机的默认变换 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FTransform CameraToPivot;

	/** 用于在枢轴空间中平滑更改摄像机平移的 Interpolator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FDoubleIIRInterpolatorVector CameraToPivotTranslationInterpolator = FDoubleIIRInterpolatorVector(4.f, 12.f);

	/** 此曲线上的 0 对应于 PitchMin（PitchLimits.X，向下看）。1 对应于最大间距 （PitchLimits.Y，向上查找）。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	UCurveVector* CameraToPivot_PitchAdjustmentCurve;

	/** 标量应用于音高调整比例曲线输出，适用于您想要制作 0 到 1 曲线形状并在此处设置幅度的情况。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	float CameraToPivot_PitchAdjustmentCurveScale = 1.f;

	/** 此曲线上的 0 对应于 CameraToPivot_SpeedAdjustment_SpeedRange.X（最小范围）。1 对应于 CameraToPivot_SpeedAdjustment_SpeedRange.Y（最大范围）。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	UCurveVector* CameraToPivot_SpeedAdjustmentCurve;

	/** 标量应用于速度调整缩放曲线输出，适用于您想要制作 0..1 曲线形状并在此处设置幅度的情况。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	float CameraToPivot_SpeedAdjustmentCurveScale = 1.f;

	/** 在基本 CameraToPivot 平移和从CameraToPivot_SpeedAdjustmentCurve曲线获得的平移之间插入摄像机到枢轴平移的速度。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
	FVector2D CameraToPivot_SpeedAdjustment_SpeedRange;


	/** 要应用于摄像机的自动跟随行为 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoFollow")
	ECameraAutoFollowMode AutoFollowMode;

	/** 启用 LazyFollow CameraAutoFollowMode 且 AllowLazyAutoFollowPitchControl 为 false 时应用的音高限制 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoFollow")
	FVector2D LazyAutoFollowPitchLimits;

	/** 启用后，在 LazyFollow 模式下将使用播放器设置的手动摄像机俯仰 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoFollow")
	bool bAllowLazyAutoFollowPitchControl = false;

	/** Higher numbers == slower following behavior, lower == faster. 0 == perfectly tight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoFollow")
	float LazyFollowLaziness = 200.f;

	/** 在手动控制摄像机后，延迟跟随行为开始之前的延迟（以秒为单位） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoFollow")
	float LazyFollowDelayAfterUserControl = 0.7f;
	
	/** 在延迟跟随行为再次生效之前，手动控制摄像机后的当前剩余时间 */
	float LazyFollowDelay_TimeRemaining = 0.f;


	/** 摄像机的视场点相对于 ViewTarget 变换的偏移量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
	FVector LookatOffsetLocal;

	/** 用于平滑更新摄像机的最终世界位置的 Interpolator （按点观察） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
	FIIRInterpolatorVector LookatWorldSpaceInterpolator = FIIRInterpolatorVector(8.f);

	/** 以前计算的摄像机在世界空间中的点的缓存 */
	FVector LastLookatWorldSpace;

	/** 启用后，相机将自动旋转以聚焦观察点。如果为 false，则相机将只直视前方。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
	uint32 bUseLookatPoint : 1;

	/** 启用后，在计算我们的目标看点时，将考虑视图目标的速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
	uint32 bDoPredictiveLookat : 1;

	/** 启用 DoPredictiveLookat 时，相机将预测注视点在未来的时间长度（以秒为单位） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt")
	float PredictiveLookatTime = 1.f;

	/** 检测相机需要避开的障碍物时使用的光线 */
	UPROPERTY(EditAnywhere, Category = "PenetrationAvoidance")
	TArray<FPenetrationAvoidanceRay> CameraPenetrationAvoidanceRays;

	/** Rays used when ensuring that the camera's safe location remains unobstructed */
	UPROPERTY(EditAnywhere, Category = "PenetrationAvoidance")
	TArray<FPenetrationAvoidanceRay> SafeLocPenetrationAvoidanceRays;

	/** Offset relative to view target to the "safe" place for the camera.  This is where penetration ray traces originate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PenetrationAvoidance")
	FVector SafeLocationOffset;

	/** Interpolator for smooth updates to the camera's safe location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PenetrationAvoidance")
	FIIRInterpolatorVector SafeLocationInterpolator = FIIRInterpolatorVector(0.f);

	/** If true, does an extra ray trace to make sure the safe loc is actually safe. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PenetrationAvoidance")
	uint32 bValidateSafeLoc:1;

	/** If true, the camera will perform ray traces to detect potential obstacles and adjust accordingly */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PenetrationAvoidance")
	uint32 bPreventCameraPenetration : 1;

	/** If true, try to detect nearby walls and move the camera in anticipation.  Helps prevent popping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PenetrationAvoidance")
	uint32 bDoPredictiveAvoidance:1;

	/** Blend time when having to bring the camera closer to the safe loc to avoid penetration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PenetrationAvoidance")
	float PenetrationBlendInTime = 0.15f;

	/** Blend time when bringing the camera away from the safe loc when space is available */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PenetrationAvoidance")
	float PenetrationBlendOutTime = 0.25f;

	/** Cache of camera safe location calculated on the previous update */
	UPROPERTY(transient)
	FVector LastSafeLocationLocal;

	/** Cache of the percentage of the distance blocked between the camera location and safe location calculated on the previous update */
	UPROPERTY(transient)
	float LastPenetrationBlockedPct = 1.f;

	/** When true, any blending of the block percentage when calculating penetration avoidance is skipped */
	bool bSkipNextPredictivePenetrationAvoidanceBlend = false;

	/**
	* Handles traces to make sure camera does not penetrate geometry and tries to find the best location for the camera.
	* Also handles interpolating back smoothly to ideal/desired position.
	*
	* @param	SafeLoc		Worst location (Start Trace)
	* @param	DesiredLocation		Desired / Ideal position for camera (End Trace)
	* @param	DeltaTime			Time passed since last frame.
	* @param	DistBlockedPct		Percentage of distance blocked last frame, between SafeLoc and CameraLoc. To interpolate out smoothly.
	* @param	bSingleRayOnly		Only fire a single ray.  Do not send out extra predictive feelers.
	*/
	virtual void PreventCameraPenetration(AActor* Target, TArray<FPenetrationAvoidanceRay>& Rays, const FVector& SafeLoc, const FVector& IdealCameraLoc, float DeltaTime, FVector& OutCameraLoc, float& DistBlockedPct, bool bPrimaryRayOnly = false);

	/** Returns the camera pivot's transform in world space */
	FTransform GetLastPivotToWorld() const
	{
		return LastPivotToWorld;
	};

	/**  Returns the pivot's transform in view target space */
	virtual FTransform GetPivotToViewTarget(const AActor* ViewTarget) const
	{
		return PivotToViewTarget;
	};

	/** Returns camera-to-pivot, before any smoothing. */
	virtual FTransform GetBaseCameraToPivot(const AActor* ViewTarget) const
	{
		return CameraToPivot;
	};

	/** Returns smoothed camera-to-pivot. */
	virtual FTransform GetCameraToPivot(const AActor* ViewTarget) const;

	/** Attempts to return view target's transform in world space */
	FTransform GetViewTargetToWorld(const AActor* ViewTarget) const;

	/** Attempts to return the view target's mesh height offset determined by the Pelvis bone's location on a character actor */
	float GetViewTargetMeshHeightOffset(const AActor* ViewTarget) const;

	/** Attempts to return the rotation of the actor that auto follow is following */
	FQuat GetAutoFollowPivotToWorldRotation(const AActor* FollowActor) const;

	/** Computes the camera pivot's goal transform in world space */
	virtual FTransform ComputePivotToWorld(const AActor* ViewTarget) const;

	/** Computes the camera's goal transform in world space */
	FTransform ComputeCameraToWorld(const AActor* ViewTarget, FTransform const& PivotToWorld) const;

	/** Computes the final FOV value during camera updates. Override as needed.*/
	virtual float ComputeFinalFOV(const AActor* ViewTarget) const
	{
		return FOV;
	}

	/** Computes the final Yaw Modifier applied to the camera's rotation. Override as needed. */
	virtual float ComputeYawModifier(const AActor* ViewTarget, float DeltaTime)
	{
		return 0.0f;
	}

	/** Computes the final Roll Modifier applied to the camera's rotation. Override as needed. */
	virtual float ComputeRollModifier(const AActor* ViewTarget, float DeltaTime)
	{
		return 0.0f;
	}

	/** Updates auto follow mode settings based on gameplay logic. Override as needed. */
	virtual void AdjustAutoFollowMode(const AActor* ViewTarget) {};

	/** Computes the camera look at point's goal position based on gameplay logic. Override as needed. */
	virtual void ComputePredictiveLookAtPoint(FVector& LookAtPointOutput, const AActor* ViewTarget, float DeltaTime);

	/** Computes the final look at point world position based on gameplay logic. Override as needed. */
	virtual FVector ComputeWorldLookAtPosition(const FVector IdealWorldLookAt, float DeltaTime);

	/** Computes the Smoothed Pivot to World location based on gameplay logic. Override as needed. */
	virtual FRotator ComputeSmoothPivotRotation(const FRotator IdealPivotToWorldRot, float DeltaTime);


	/** Cache of previous pivot to world transform post-smoothing */
	FTransform LastPivotToWorld;

	/** Cache of previous pivot to world transform pre-smoothing */
	FTransform LastUnsmoothedPivotToWorld;

	/** Cache of previous owner PlayerController's control rotation */
	FRotator LastControlRotation;	


	/** When enabled, debug visuals will be rendered for the camera pivot point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugPivot = false;

	/** When enabled, debug visuals will be rendered for the camera look at point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugLookat = false;

	/** When enabled, debug visuals will be rendered for the camera safe location, where penetration avoidance ray traces originate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugSafeLoc = false;

	/** When enabled, debug visuals will be rendered for the penetration avoidance ray traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugPenetrationAvoidance = false;

};
