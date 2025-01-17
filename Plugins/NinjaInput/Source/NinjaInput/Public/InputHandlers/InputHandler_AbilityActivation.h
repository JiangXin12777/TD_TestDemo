// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "NinjaInputHandler.h"
#include "UObject/Object.h"
#include "InputHandler_AbilityActivation.generated.h"

class UGameplayAbility;

/**
 * 用于能力激活的基本输入处理程序。
 */
UCLASS(Abstract)
class NINJAINPUT_API UInputHandler_AbilityActivation : public UNinjaInputHandler
{
    
    GENERATED_BODY()

public:
    
    UInputHandler_AbilityActivation();
    
protected:

    /**
     * 确定是否应切换激活。
     *
     * 这意味着第一个成功的触发将激活该能力，下一个将首先检查该能力是否处于活动状态（带有可选的查询测试），如果是，则会中断该能力。
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
    bool bToggledActivation;

    /**
     * 如果设置为 true，此处理程序将在该功能处于活动状态时在其他输入上发送游戏事件。
     *
     * 激活必须由具有自己的激活策略（即类、标签或输入 ID）的子类确定，并且必须使用这些相同的策略来确定能力是否处于活动状态。
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
    bool bSendEventIfActive;

    /**
     * 如果设置为 true，该事件将在本地触发。
     *
     * 如果启用了“在服务器上触发事件”标志并且输入所有者既受本地控制又具有权威，则保证事件不会被触发两次。
     */    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation", meta = (EditCondition = "bSendEventIfActive", EditConditionHides))
    bool bTriggerEventLocally;

    /**
     * 如果设置为true，该事件将在服务器（权威）版本上触发。
     *
     * 如果启用“本地触发事件”标志并且输入所有者既受本地控制又具有权威，则保证事件不会被触发两次。
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation", meta = (EditCondition = "bSendEventIfActive", EditConditionHides))
    bool bTriggerEventOnServer;
    
    /**
     * 游戏标签用于在能力处于活动状态时触发游戏事件（如果有激活尝试）。
     *
     * 该游戏事件可以通过活动能力或任何其他已经处于活动状态或可以由游戏状态激活的能力来跟踪。一个常见的用例是组合攻击。
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation", meta = (EditCondition = "bSendEventIfActive", EditConditionHides))
    FGameplayTag ActiveEventTag;

protected:
    // ~Begin UNinjaInputHandler Interface
    virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction) const override;
    virtual void HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction) const override;
    virtual void HandleCancelledEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction) const override;
    // ~End UNinjaInputHandler Interface

    /**
     * 允许根据当前设置正确处理已处理的能力。
     *
     * 您重写此方法的一个原因是，如果没有理由这样做，可能会避免迭代功能。例如，默认情况下，“事件”和“切换”检查都会被考虑在内。
     * 如果您的系统永远不会发生这种情况，那么调整此功能以满足您的需求可能是值得的。
     *
     * @param Manager           已调用此处理程序的输入管理器。必须有效。
     * @param Value             从增强输入系统收到的原始值。
     * @param InputAction       触发此激活的原始操作。
     * @return                  如果此方法处理该能力，则为 true。如果为 false，则处理待处理。
     */
    virtual bool TryHandleActiveAbility(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction) const;

    /**
     * 具体实现可以根据特定标准检查某个能力是否处于活动状态。
     *
     * 每个子类必须正确实现此方法，而不调用基本实现（super）。
     * 
     * @param Manager           已调用此处理程序的输入管理器。必须有效。
     * @return                  True if an Ability is active.
     */
    virtual bool HasActiveAbility(UNinjaInputManagerComponent* Manager) const;

    /**
     * Concrete implementation that will activate an ability using a proper activation mode.
     *
     * Each subclass must implement this method properly, without calling the base implementation (super).
     * 
     * @param Manager           Input Manager that has invoked this handler. Must be valid.
     * @param Value             Original value received from the Enhanced Input System.
     * @param InputAction       Original Action that is triggering this activation.
     */
    virtual void ActivateAbility(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
        const UInputAction* InputAction) const;

    /**
     * Concrete implementation that will cancel an ability using a proper cancellation mode.
     * 
     * @param Manager           Input Manager that has invoked this handler. Must be valid.
     * @param Value             Original value received from the Enhanced Input System.
     * @param InputAction       Original Action that is triggering this activation.
     */
    virtual void CancelAbility(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
        const UInputAction* InputAction) const;
    
    /**
     * Sends a gameplay event for the provided tag.
     *
     * It will send the Input Action as an optional object and the value as Magnitude.
     *
     * @param Manager           Input Manager that has invoked this handler. Must be valid.
     * @param GameplayEventTag  Gameplay Event Tag that identifies the Event to trigger. Must be valid.
     * @param Value             Input value that generated the input activation.
     * @param InputAction       Specific Input Action that was activated.
     * @return                  Ability activations generated by this event. 
     */
    UFUNCTION(BlueprintCallable, Category = "Ability Activation Input Handler")
    virtual int32 SendGameplayEvent(const UNinjaInputManagerComponent* Manager, FGameplayTag GameplayEventTag,
        const FInputActionValue& Value, const UInputAction* InputAction) const;
    
};
