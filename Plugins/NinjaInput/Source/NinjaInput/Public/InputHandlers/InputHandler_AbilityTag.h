// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputHandler_AbilityActivation.h"
#include "InputHandler_AbilityTag.generated.h"

/**
 * 使用游戏标签触发和中断游戏能力。
 *
 * 跟踪触发事件，该事件基本上将激活一项能力，但它也准备处理完成和取消事件，这反过来又会中断该能力。
 */
UCLASS(DisplayName = "GAS: Activate Ability by Tags")
class NINJAINPUT_API UInputHandler_AbilityTag : public UInputHandler_AbilityActivation
{
	
	GENERATED_BODY()

public:

	UInputHandler_AbilityTag();

	virtual bool CanHandle_Implementation(const ETriggerEvent& TriggerEvent,
	    const UInputAction* InputAction) const override;
	
protected:

	/** 用于触发一项或多项能力的游戏标签。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	FGameplayTagContainer AbilityTags;

	/**
	 * 用于过滤掉不应取消的能力的标签。
	 * 
	 * 这些与两种情况相关：
	 * 1. 切换已启用并且该能力正在被中断。
	 * 2. 跟踪已完成或已取消状态，这将中断该能力。
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	FGameplayTagContainer CancelFilterTags;
	
    // ~Begin UInputHandler_AbilityActivation Interface
    virtual bool HasActiveAbility(UNinjaInputManagerComponent* Manager) const final override;
    virtual void ActivateAbility(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction) const final override;
    virtual void CancelAbility(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction) const final override;
    // ~End UInputHandler_AbilityActivation Interface
};
