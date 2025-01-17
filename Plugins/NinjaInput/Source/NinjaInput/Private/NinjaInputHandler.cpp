// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "NinjaInputHandler.h"

#include "AbilitySystemComponent.h"
#include "NinjaInputManagerComponent.h"
#include "NinjaInputSettings.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY(LogNinjaInputHandler);

UNinjaInputHandler::UNinjaInputHandler()
{
    bCanBeBuffered = false;
}

UWorld* UNinjaInputHandler::GetWorld() const
{
    return WorldPtr.IsValid() && WorldPtr->IsValidLowLevelFast() ? WorldPtr.Get() : nullptr;
}

bool UNinjaInputHandler::CanHandle_Implementation(const ETriggerEvent& TriggerEvent,
    const UInputAction* InputAction) const
{
	return TriggerEvent != ETriggerEvent::None && IsValid(InputAction) &&
		InputActions.Contains(InputAction) && TriggerEvents.Contains(TriggerEvent);
}

void UNinjaInputHandler::HandleInput(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
    const ETriggerEvent& TriggerEvent, const UInputAction* InputAction) const
{
	check(IsValid(Manager));

    AddOnScreenDebugMessage(Manager, Value, TriggerEvent, InputAction);
    
	switch (TriggerEvent)
	{
	case ETriggerEvent::Triggered:
		HandleTriggeredEvent(Manager, Value, InputAction);
		break;
	case ETriggerEvent::Started:
		HandleStartedEvent(Manager, Value, InputAction);
		break;
	case ETriggerEvent::Ongoing:
		HandleOngoingEvent(Manager, Value, InputAction);
		break;
	case ETriggerEvent::Canceled:
		HandleCancelledEvent(Manager, Value, InputAction);
		break;
	case ETriggerEvent::Completed:
		HandleCompletedEvent(Manager, Value, InputAction);
		break;
	default:
		const UEnum* EnumPtr = FindObject<UEnum>(GetOuter(), TEXT("ETriggerEvent"), true);
		UE_LOG(LogNinjaInputHandler, Warning, TEXT("[%s] Unable to handle Trigger Event '%s'."),
			*GetNameSafe(Manager->GetOwner()), *EnumPtr->GetValueAsString(TriggerEvent));
	}
}

bool UNinjaInputHandler::CanBeBuffered() const
{
    return bCanBeBuffered;
}

void UNinjaInputHandler::SetWorld(UWorld* WorldReference)
{
    WorldPtr = WorldReference;
}

bool UNinjaInputHandler::HasAnyTags(const UNinjaInputManagerComponent* Manager, const FGameplayTagContainer& Tags)
{
	check(Manager);

	if (Tags.IsEmpty() == false)
	{
		const UAbilitySystemComponent* AbilityComponent = Manager->GetAbilitySystemComponent();
		return IsValid(AbilityComponent) && AbilityComponent->HasAnyMatchingGameplayTags(Tags);
	}

	return false;
}

void UNinjaInputHandler::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
}

void UNinjaInputHandler::HandleStartedEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
}

void UNinjaInputHandler::HandleOngoingEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
}

void UNinjaInputHandler::HandleCancelledEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
}

void UNinjaInputHandler::HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
}

void UNinjaInputHandler::AddOnScreenDebugMessage(const UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
    const ETriggerEvent& TriggerEvent, const UInputAction* InputAction) const
{
    #if WITH_EDITOR
    if (IsValid(GEngine))
    {
        if (GetDefault<UNinjaInputSettings>()->bShowScreenDebugMessages)
        {
            const UEnum* EnumPtr = FindObject<UEnum>(GetOuter(), TEXT("ETriggerEvent"), true);
            const FString DebugMessage = FString::Printf(TEXT("Input Handler Debug: { Class: '%s', Owner: '%s', Action: '%s', Trigger: '%s', Value: '%s' }"),
                *GetNameSafe(this), *GetNameSafe(Manager->GetOwner()), *GetNameSafe(InputAction), *EnumPtr->GetValueAsString(TriggerEvent), *Value.ToString());

            const float Duration = GetDefault<UNinjaInputSettings>()->DebugMessageDuration;
            const FColor DebugColor = GetDefault<UNinjaInputSettings>()->DebugMessageColor;
            GEngine->AddOnScreenDebugMessage(INDEX_NONE, Duration, DebugColor, DebugMessage);
        }
    }
    #endif
}

#if WITH_EDITOR
bool UNinjaInputHandler::HasAnyInputActions() const
{
    return !InputActions.IsEmpty();
}

bool UNinjaInputHandler::HasAnyTriggerEvents() const
{
    return !TriggerEvents.IsEmpty();
}

bool UNinjaInputHandler::HandlesAction(const UInputAction* InputAction) const
{
    return InputActions.Contains(InputAction);
}
#endif