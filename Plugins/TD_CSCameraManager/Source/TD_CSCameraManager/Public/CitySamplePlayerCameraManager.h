#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"

#include "CitySamplePlayerCameraManager.generated.h"

class ACitySamplePlayerController;
class UCineCameraComponent;

/**
 * 为便于使用，列举了默认相机模式的等效项
 */
UENUM(BlueprintType)
enum class EDebugCameraStyle : uint8
{
	None,

	/** 固定 */
	Fixed,

	/** 第三人称视角 */
	ThirdPerson,

	/** 自由相机模式 */
	FreeCam,

	/** 自由相机模式 */
	FreeCam_Default,

	/** 第一人称视角 */
	FirstPerson,
};


/**
 * 管理器当前正在混合的活动摄像机的表示
 */
USTRUCT()
struct FActiveCitySampleCamera
{
	GENERATED_BODY()

public:
	/** 指向相机实例的指针 */
	UPROPERTY()
	UCitySampleCameraMode* Camera = nullptr;

	/** 指向当前视图目标的指针 */
	UPROPERTY()
	AActor* ViewTarget = nullptr;

	/** 对于传入摄像机模式，朝 1 方向插值，对于其他模式，朝 0 方向插值。 */
	float TransitionAlpha = 0.f;

	/** 相机过渡时的更新速率 */
	float TransitionUpdateRate = 0.f;

	/** 混合堆栈中的当前混合权重 */
	float BlendWeight = 0.f;

	/** CitySampleCameraModeInstance 与此活动相机对象相关联，实例 ID */
	int32 InstanceIndex = INDEX_NONE;

	/** 缓存摄像机上一视图信息 */
	FMinimalViewInfo LastPOV;

	/** 如果为 true，则在涉及此摄像机的摄像机过渡期间将锁定视图信息 */
	bool bLockOutgoingPOV = false;
};

/** 可用于支持活动摄像机的摄像机模式实例 */
USTRUCT(BlueprintType)
struct FCitySampleCameraModeInstance
{
	GENERATED_BODY()

public:

	/** 与实例关联的 Camera mode 类 */
	UPROPERTY()
	TSubclassOf<class UCitySampleCameraMode> CameraModeClass;

	/** 查看实例正在关注的目标（相机视口聚焦的对象） */
	UPROPERTY()
	AActor* ViewTarget = nullptr;

	/** 与实例关联的 Camera mode 对象 */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly)
	UCitySampleCameraMode* CameraMode = nullptr;

	/** 与实例关联的 Cine cam 组件 */
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly)
	UCineCameraComponent* CineCameraComponent = nullptr;

	/** 触发与实例关联的底层摄像机模式的更新 */
	void UpdateCamera(float DeltaTime, FTViewTarget& OutVT);
};


/**
 * 管理器当前正在混合的活动摄像机的表示
 */
UCLASS()
class TD_CSCAMERAMANAGER_API ACitySamplePlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ACitySamplePlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	
	//~ Begin APlayerCameraManager Interface
	virtual void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime) override;
	virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	//~ End APlayerCameraManager Interface

	/** 返回摄像机混合堆栈顶部的摄像机正在过渡到的视图信息 */
	FMinimalViewInfo GetTransitionGoalPOV() const
	{
		return TransitionGoalPOV;
	}

	/** 应用预制的摄像机样式以进行调试 */
	// UFUNCTION(BlueprintCallable, Category = Camera)
	// void SetDebugCameraStyle(EDebugCameraStyle NewDebugCameraStyle);

	/** 将视图间距限制设置为传入的值 */
	UFUNCTION(BlueprintCallable)
	void SetViewPitchLimits(float MinPitch, float MaxPitch);

	/** 将视图间距限制设置为最初在类默认值中设置的值 */
	UFUNCTION(BlueprintCallable)
	void ResetViewPitchLimits();

	/** 骨盆 Z 高度，在组件空间中。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BasePelvisRelativeZ;

	/** 充当摄像机视图目标的角色的骨盆骨骼名称 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PelvisBoneName;

	/** 启用备用相机时使用的 Camera mode */
	TSubclassOf<UCitySampleCameraMode> AltCameraMode;

	/** 使用备用相机模式时查看要对焦的目标 */
	UPROPERTY(transient)
	AActor* AltViewTarget;

	/** 是时候过渡到备用相机模式了(过度到新相机得时间) */
	float AltCameraTransitionTime;

	/** 查看退出备用相机模式时要返回的目标 */
	UPROPERTY(transient)
	AActor* SavedMainViewTarget;

protected:
	/** 开始从主摄像机过渡到当前配置的备用相机设置 */
	// void TransitionToAltCamera();

	/** 开始从备用相机过渡到当前配置的主摄像机设置 */
	// void TransitionFromAltCamera();

	/** 更新与传入的索引对应的单个照相机模式 */
	void UpdateCameraInStack(int32 StackIdx, float DeltaTime, FTViewTarget& OutVT);

	/** 返回由我们正在过渡的摄像机模式确定的过渡时间 */
	float GetModeTransitionTime(UCitySampleCameraMode* ToMode) const;

	/** 管理器将尝试在其之间混合的活动摄像机堆栈 */
	UPROPERTY(Transient)
	TArray<struct FActiveCitySampleCamera> CameraBlendStack;

	/** 可用相机模式实例列表 */
	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly)
	TArray<FCitySampleCameraModeInstance> CameraModeInstances;

private:
	/** 尝试查找现有相机模式实例以设置新的视图目标，如果未找到现有候选对象，则创建新的相机模式实例 */
	int32 FindOrCreateCameraModeInstance(TSubclassOf<UCitySampleCameraMode> CameraModeClass, AActor* InViewTarget);

	/** 活动转移的目标 POV */
	FMinimalViewInfo TransitionGoalPOV;

	/** 默认起始最小音高限制值 */
	float DefaultMinPitchLimit = 0.0f;

	/** 默认起始最大螺距限制值 */
	float DefaultMaxPitchLimit = 0.0f;
};

