// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "FBufferedInputCommand.generated.h"

class UNinjaInputManagerComponent;
class UInputAction;
class UNinjaInputHandler;

/**
 * Represents a buffered command that may be executed later.
 */
USTRUCT(BlueprintType)
struct FBufferedInputCommand
{
    
    GENERATED_BODY()

    /** The manager that registered this command. */
    UPROPERTY(BlueprintReadOnly, Category = "Input Command")
    TObjectPtr<UNinjaInputManagerComponent> Source;
    
    /** Input Action that will aggregate all commands. */
    UPROPERTY(BlueprintReadOnly, Category = "Input Command")
    TObjectPtr<const UInputAction> InputAction;

    /** Handler that must be executed. */
    UPROPERTY(BlueprintReadOnly, Category = "Input Command")
    TObjectPtr<const UNinjaInputHandler> Handler;

    /** The value that needs to be processed. */
    UPROPERTY(BlueprintReadOnly, Category = "Input Command")
    FInputActionValue Value;

    /** Trigger that originated the action. */
    UPROPERTY(BlueprintReadOnly, Category = "Input Command")
    ETriggerEvent TriggerEvent;

    FBufferedInputCommand()
    {
    	Source = nullptr;
        InputAction = nullptr;
        Handler = nullptr;
        Value.Reset();
        TriggerEvent = ETriggerEvent::None;
    }

    explicit FBufferedInputCommand(UNinjaInputManagerComponent* Source
        , const UInputAction* Action
        , const UNinjaInputHandler* Handler
        , const FInputActionValue& Value
        , const ETriggerEvent TriggerEvent)
        : Source(Source)
        , InputAction(Action)
        , Handler(Handler)
        , Value(Value)
        , TriggerEvent(TriggerEvent)
    {
    }

    /**
     * Evaluates if this command has valid data and can be executed.
     */
    bool IsValid() const;
    
    /**
     * Executes the command represented by this entry, on a given context.
     */
    void Execute() const;

    // Within the context of a buffered command, we can to determine equality by comparing the
    // action that should be handled, its trigger and the Handler that is supposed to execute.
    FORCEINLINE bool operator == (const FBufferedInputCommand& In) const
    {
        return In.InputAction == InputAction && In.Handler == Handler && In.TriggerEvent == TriggerEvent;
    }

    // Within the context of a buffered command, we can determine inequality by comparing the
    // action that should be handled, its trigger and the Handler that is supposed to execute.
    FORCEINLINE bool operator != (const FBufferedInputCommand& In) const
    {
        return In.InputAction != InputAction || In.Handler != Handler || In.TriggerEvent != TriggerEvent;
    }
};