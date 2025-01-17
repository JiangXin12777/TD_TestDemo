// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "NinjaInputBufferComponent.h"
#include "Types/FProcessedBinding.h"
#include "Types/FProcessedInputSetup.h"
#include "NinjaInputManagerComponent.generated.h"

class APawn;
class UEnhancedInputComponent;
class UArrowComponent;
class UEnhancedInputLocalPlayerSubsystem;
class UNinjaInputHandler;
class UNinjaInputSetupDataAsset;
class UInputMappingContext;
class UInputAction;

// Log category fo the Input Manager Component.
//
// This component can output a lot of verbose/very verbose information and if you are interested in
// using these messages for debugging, you can enable this category in your DefaultEngine.ini file:
//
// [Core.Log]
// global=log
// LogNinjaManagerComponent=VeryVerbose
//
DECLARE_LOG_CATEGORY_EXTERN(LogNinjaInputManagerComponent, Log, All);

/**
 * Manages Input Contexts and delegates Input Actions to Input Handlers registered to this component.
 */
UCLASS(ClassGroup = NinjaInput, meta = (BlueprintSpawnableComponent))
class NINJAINPUT_API UNinjaInputManagerComponent : public UNinjaInputBufferComponent, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/**
	 * The tag name used to find Arrow Components that represent a forward direction.
	 * 
	 * This is useful for top-down or similar games, where the controller or actor direction
	 * vectors won't properly reflect the direction should move towards.
	 */
	static FName ForwardReferenceTag;
	
	UNinjaInputManagerComponent();

	virtual void OnRegister() override;
    virtual void OnUnregister() override;

	/** 注册当前所有者所装载的技能组件 */
	//UFUNCTION(BlueprintCallable, Category = "TD|Input Component")
	//void RegisterOwnerAbilitySystemComponent(UAbilitySystemComponent* InVal);

	/**
	 * Provides the owner's Forward Reference.
	 */
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Ninja Input|Manager Component")
	FVector GetForwardVector() const;

	/**
	 * Provides the owner's Right Reference.
	 */
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Ninja Input|Manager Component")
	FVector GetRightVector() const;

    /**
     * Provides the last input vector handled by the owner.
     *
     * By default, it will retrieve the value from the owner's Pawn Movement Component
     * but this can be overriden by implementing the "Last Input Vector Provider Interface"
     * in the Pawn related to this component.
     *
     * @return
     *      The last input vector that was handled by the owning pawn.
     */
    UFUNCTION(BlueprintPure, Category = "Ninja Input|Manager Component")
    FVector GetLastInputVector() const;
    
	/**
	 * Provides the pawn that owns this component.
	 *
	 * This function is able to retrieve the pawn even if this component is attached to actors
	 * that are not pawns, such as Controllers or Player States.
	 *
	 * @return
	 *		The Pawn that owns this component. It may be the owning actor or the pawn related
	 *		to a Controller or to a Player State. It will ultimately ensure that is not null.
	 */
	UFUNCTION(BlueprintPure, Category = "Ninja Input|Input Manager Component")
	APawn* GetPawn() const;

	/**
	 * Provides the controller that owns this component.
	 *
	 * This function is able to retrieve the controller, regardless of the actor that owns
	 * it, which may be a Pawn, an Controller or a Player State.
	 *
	 * @return
	 *		The Controller that owns this component. It may be retrieved from the owning Pawn,
	 *		Player State or the owning controller itself. It will ensure that is not null.
	 */
	UFUNCTION(BlueprintPure, Category = "Ninja Input|Input Manager Component")
	AController* GetController() const;

    /**
     * Checks if this component is running with a local controller.
     *
     * @return
     *      A boolean informing if this Manager Component has a locally-controlled owner.
     */
    UFUNCTION(BlueprintPure, Category = "Ninja Input|Manager Component")
    bool IsLocallyControlled() const;
    
	/**
	 * Provides the Ability System Component from this component's owner.
	 *
	 * Please note that this component is a valid implementation of the Ability System Interface.
	 *
	 * @return
	 *      The Ability System Component, provided by the owner, via the ASC Interface.
	 */
	UFUNCTION(BlueprintPure, Category = "Ninja Input|Input Manager Component")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    
    /**
     * Retrieves the Enhanced Input Subsystem for the provided controller.
     *
     * @return
     *      The Enhanced Input Subsystem. retrieved from the Controller's Local Player.
     */
    UFUNCTION(BlueprintPure, Category = "Ninja Input|Manager Component")
    UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem(AController* Controller) const;
    
    /**
     * Checks if a given Setup Data ir registered to this component.
     *
     * @param SetupData
     *      Setup Data Asset to be evaluated.
     *
     * @return
     *		A boolean value informing if the provided Setup Data was already
     *		registered to this component. 
     */
    UFUNCTION(BlueprintPure, Category = "Ninja Input|Input Manager Component")
    bool HasSetupData(const UNinjaInputSetupDataAsset* SetupData) const;
    
	/**
	 * Checks if a given Input Mapping Context is registered for the current owner.
	 *
	 * @param InputMappingContext
	 *		Input Mapping Context to be checked. Must be provided.
	 * 
	 * @return
	 *		A boolean value informing if the provided Mapping Context was already
	 *		registered to the current Local Player.
	 */
	UFUNCTION(BlueprintPure, Category = "Ninja Input|Input Manager Component")
	bool HasInputMappingContext(const UInputMappingContext* InputMappingContext) const;

    /**
     * Checks if this component has a handler compatible with a given Action/Trigger Event.
     *
     * @param InputAction
     *      Input Action to be checked. Must be provided.
     *
     * @param TriggerEvent
     *      Trigger Event compatible with the action. Must be provided.
     *
     * @return
     *      A boolean value informing if the providing combination of Input Action and
     *      Trigger Event can be dispatched to any of the internal Input Handlers.
     */
    UFUNCTION(BlueprintPure, Category = "Ninja Input|Input Manager Component")
    bool HasCompatibleInputHandler(const UInputAction* InputAction, const ETriggerEvent& TriggerEvent) const;

    /**
     * Processes a Setup Data, registering its Input Contexts and Handlers.
     *
     * @param SetupData
     *      The new setup data to be added. If this setup Data was already registered, then
     *      it will be safely ignored, without errors being thrown.
     */
    UFUNCTION(BlueprintCallable, Category = "Ninja Input|Input Manager Component")
    void AddInputSetupData(const UNinjaInputSetupDataAsset* SetupData);

    /**
     * Removes a Setup Data previously registered.
     *
     * @param SetupData
     *      Setup data to be removed from this component. If not registered, then nothing
     *      will happen and no errors will be thrown.
     */
    UFUNCTION(BlueprintCallable, Category = "Ninja Input|Input Manager Component")
    void RemoveInputSetupData(const UNinjaInputSetupDataAsset* SetupData);
    
    /**
     * Sends a Gameplay Event to the owner's ASC.
     *
     * @param GameplayEventTag
     *      Gameplay Tag used to identify the event.
     *
     * @param Value
     *      Input value that has triggered this event. It will be provided in the event's
     *      Payload as the event's value for its Magnitude attribute.
     *
     * @param InputAction
     *      The Input Action that has triggered this event. It will be provided in the event's
     *      Payload as the first Optional Object.
     *
     * @param bSendLocally
     *      Determines if this event should be triggered locally, in case of a non-authoritative
     *      local Player Controller. Defaults to true.
     *
     * @param bSendToServer
     *      Determines if this event should be triggered on the server in case of a local Player
     *      Controller not having authoritative privileges over the actor. Defaults to true.
     *
     * @return
     *      The number of Ability Activations triggered by this event. The number is only accurate
     *      for the local activations, but further activations may happen after an eventual RPC
     *      is sent out, if necessary, to the authoritative version.
     */
    UFUNCTION(BlueprintCallable, Category = "Ninja Input|Input Manager Component")
    int32 SendGameplayEventToOwner(const FGameplayTag& GameplayEventTag, const FInputActionValue& Value,
        const UInputAction* InputAction, bool bSendLocally = true, bool bSendToServer = true) const;
    
protected:

	/** The input component bound to this manager. */
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> InputComponent;

    /**
     * All data assets that will contribute to this Input Manager's setup.
     *
     * Data Assets are unique and the component ensures they are not duplicated in
     * this array. There are exposed functions to check for specific Data Assets and
     * also to register/unregister them.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TArray<TObjectPtr<UNinjaInputSetupDataAsset>> InputHandlerSetup;

    /**
     * Entrypoint for the component's initialization. 
     */
    void SetupInputComponent(const APawn* Pawn);
    
    /**
     * Registers a new Input Mapping Context and process necessary bindings.
     *
     * @param InputMappingContext
     *      Setup Data Asset that is providing the Mapping Context to be registered.
     *      
     * @param Priority
     *		Priority assigned to the new context. As per the Enhanced Input Component,
     *		higher priority contexts will be processed first.
     *
     * @param OutBindings
     *      Output parameter containing all generated bindings.
     */
    void AddInputMappingContext(const UInputMappingContext* InputMappingContext, int32 Priority, TArray<FProcessedBinding>& OutBindings);

    /**
     * Dispatches an action to a registered Input Handler for the Started Event.
     *
     * We need to differentiate the event in the function level, as just relying on the Action
     * Instance may lead to incorrect triggered, since multiple events can happen in the same frame.
     * 
     * @param ActionInstance
     *      Details about the Action that must be processed.
     */
    UFUNCTION()
    void DispatchStartedEvent(const FInputActionInstance& ActionInstance);

    /**
     * Dispatches an action to a registered Input Handler for the Started Event.
     *
     * We need to differentiate the event in the function level, as just relying on the Action
     * Instance may lead to incorrect triggered, since multiple events can happen in the same frame.
     * 
     * @param ActionInstance
     *      Details about the Action that must be processed.
     */
    UFUNCTION()
    void DispatchTriggeredEvent(const FInputActionInstance& ActionInstance);

    /**
     * Dispatches an action to a registered Input Handler for the Ongoing Event.
     *
     * We need to differentiate the event in the function level, as just relying on the Action
     * Instance may lead to incorrect triggered, since multiple events can happen in the same frame.
     * 
     * @param ActionInstance
     *      Details about the Action that must be processed.
     */
    UFUNCTION()
    void DispatchOngoingEvent(const FInputActionInstance& ActionInstance);

    /**
     * Dispatches an action to a registered Input Handler for the Ongoing Event.
     *
     * We need to differentiate the event in the function level, as just relying on the Action
     * Instance may lead to incorrect triggered, since multiple events can happen in the same frame.
     * 
     * @param ActionInstance
     *      Details about the Action that must be processed.
     */
    UFUNCTION()
    void DispatchCompletedEvent(const FInputActionInstance& ActionInstance);

    /**
     * Dispatches an action to a registered Input Handler for the Ongoing Event.
     *
     * We need to differentiate the event in the function level, as just relying on the Action
     * Instance may lead to incorrect triggered, since multiple events can happen in the same frame.
     * 
     * @param ActionInstance
     *      Details about the Action that must be processed.
     */
    UFUNCTION()
    void DispatchCancelledEvent(const FInputActionInstance& ActionInstance);
    
    /**
     * Effectively dispatches the event, using the Actual Trigger.
     *
     * @param ActionInstance
     *      Details about the Action that must be processed.
     * @param ActualTrigger
     *      The actual trigger that's being handled. The one contained in the Action Instance can
     *      be misleading as multiple triggers can happen on the same frame.
     */
    void Dispatch(const FInputActionInstance& ActionInstance, ETriggerEvent ActualTrigger);
    
    /**
     * Removes an Input Mapping Context and its bindings.
     *
     * @param InputMappingContext
     *		Context to be removed from the Enhanced Input Subsystem and this manager.
     */
    void RemoveInputMappingContext(const UInputMappingContext* InputMappingContext);

    /**
     * Clears the entire Input Setup assigned to this component.
     */
    virtual void ClearInputSetup();
    
	/**
	 * Invoked when the owning Pawn restarts, allowing this component to recreate the bindings.
	 */
	UFUNCTION()
	void OnPawnRestarted(APawn* Pawn);

	/**
	 * Invoked when the pawn's controller changes, allowing this component to clear current bindings.
	 */
	UFUNCTION()
	void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);	

    /**
     * Provides a vector reference for a given axis.
     *
     * This is necessary for all handlers that requires movement direction such as "Movement".
     */
    UFUNCTION(BlueprintNativeEvent, Category = "Input Manager Component")
    void GetVectorForAxis(const EAxis::Type Axis, FVector& OutReference) const;

private:

	/** Controller currently assigned to our owner. */
	UPROPERTY()
	TObjectPtr<AController> OwnerController;

	/** 当前所有者的技能组件 */
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/** Forward reference for the current owner. */
	UPROPERTY()
	TObjectPtr<UArrowComponent> ForwardReference;

    /** All setups registered to this component, mapped by their Mapping Context.*/
    UPROPERTY()
    TMap<TObjectPtr<UInputMappingContext>, FProcessedInputSetup> ProcessedSetups;
    
    /**
     * Allows sending a gameplay event to server when we are a local autonomous proxy.
     */
    UFUNCTION(Server, Reliable)
    void Server_SendGameplayEventToOwner(const FGameplayTag& GameplayEventTag,
        const FInputActionValue& Value, const UInputAction* InputAction) const;
    
    /**
     * Allows sending a gameplay event to client when we are a remote server (!).
     * This is a very unlikely scenario, just added for the sake of being thorough.
     */
    UFUNCTION(Client, Reliable)
    void Client_SendGameplayEventToOwner(const FGameplayTag& GameplayEventTag,
        const FInputActionValue& Value, const UInputAction* InputAction) const;
    
};
