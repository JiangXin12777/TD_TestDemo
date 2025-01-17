// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "NinjaInputManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "NinjaInputHandler.h"
#include "NinjaInputHandlerHelpers.h"
#include "NinjaInputSettings.h"
#include "Components/ArrowComponent.h"
#include "Data/NinjaInputSetupDataAsset.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/LastInputProviderInterface.h"

DEFINE_LOG_CATEGORY(LogNinjaInputManagerComponent);

FName UNinjaInputManagerComponent::ForwardReferenceTag = TEXT("InputForwardReference");

UNinjaInputManagerComponent::UNinjaInputManagerComponent()
{
    SetIsReplicatedByDefault(true);
}

// Initialization and Shutdown ------------------------------------------------

void UNinjaInputManagerComponent::OnRegister()
{
	Super::OnRegister();

	const UWorld* World = GetWorld();
	if (IsValid(World) && World->IsGameWorld())
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (IsValid(OwnerPawn))
		{
			OwnerPawn->ReceiveRestartedDelegate.AddDynamic(this, &ThisClass::OnPawnRestarted);
			OwnerPawn->ReceiveControllerChangedDelegate.AddDynamic(this, &ThisClass::OnControllerChanged);

			if (IsValid(OwnerPawn->InputComponent))
			{
				// If our pawn already has an Input Component, then this means this component
				// was added/registered after a pawn restart. So we need to initialize it right away!
				
				OnPawnRestarted(OwnerPawn);
			}
		}
	}
}

// ReSharper disable CppParameterMayBeConstPtrOrRef
void UNinjaInputManagerComponent::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
    // Only handle releasing the controller as "OnPawnRestarted" will re-run the setup.
    if (ensure(IsValid(Pawn) && Pawn == GetOwner()) && OldController)
    {
        ClearInputSetup();

        if (IsValid(NewController)) { OwnerController = NewController; }
        else { OwnerController = nullptr; }
    }
}
// ReSharper restore CppParameterMayBeConstPtrOrRef

// ReSharper disable CppParameterMayBeConstPtrOrRef
void UNinjaInputManagerComponent::OnPawnRestarted(APawn* Pawn)
{
    if (ensure(Pawn && Pawn == GetPawn()) && Pawn->InputComponent)
    {
        if (Pawn->InputComponent)
        {
            OwnerController = Pawn->GetController();
            SetupInputComponent(Pawn);
        }

        const TArray<UActorComponent*> ForwardReferences = GetOwner()->GetComponentsByTag(UArrowComponent::StaticClass(), ForwardReferenceTag);
        ForwardReference = ForwardReferences.Num() > 0 ? Cast<UArrowComponent>(ForwardReferences[0]) : nullptr;
        if (ForwardReference)
        {
            UE_LOG(LogNinjaInputManagerComponent, Log, TEXT("[%s] Retrieved a new Forward Reference (%d available)."),
                *GetNameSafe(GetOwner()), ForwardReferences.Num());
        }
    }
}
// ReSharper restore CppParameterMayBeConstPtrOrRef

void UNinjaInputManagerComponent::OnUnregister()
{
    const TObjectPtr<UWorld> World = GetWorld();
    if (IsValid(World) && World->IsGameWorld())
    {
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (IsValid(OwnerPawn))
        {
            OwnerPawn->ReceiveRestartedDelegate.RemoveAll(this);
            OwnerPawn->ReceiveControllerChangedDelegate.RemoveAll(this);
        }
    }

    ClearInputSetup();
    
    Super::OnUnregister();
}

void UNinjaInputManagerComponent::SetupInputComponent(const APawn* Pawn)
{
	InputComponent = Cast<UEnhancedInputComponent>(Pawn->InputComponent);
	if (ensureMsgf(InputComponent, TEXT("Please make sure to use the EnhancedInputComponent!")))
	{
		if (IsValid(OwnerController))
		{
		    for (const TObjectPtr<const UNinjaInputSetupDataAsset> SetupData : InputHandlerSetup)
		    {
		        AddInputSetupData(SetupData);
		    }
		}
	}
}

// Core Functionality -------------------------------------------------------------------

void UNinjaInputManagerComponent::AddInputSetupData(const UNinjaInputSetupDataAsset* SetupData)
{
    check(SetupData);

    // Make sure that this setup has not been already processed.
    if (HasSetupData(SetupData))
    {
        UE_LOG(LogNinjaInputManagerComponent, Warning, TEXT("[%s] Received a duplicated Input Setup (%s)!"),
             *GetNameSafe(GetOwner()), *GetNameSafe(SetupData));
        
        return;
    }
    
    // Also make sure we don't have the same IMC from another Setup.
    const TObjectPtr<UInputMappingContext> NewContext = SetupData->InputMappingContext;
    if (HasInputMappingContext(NewContext))
    {
        const FProcessedInputSetup OriginalSetup = *ProcessedSetups.Find(NewContext);
        UE_LOG(LogNinjaInputManagerComponent, Warning, TEXT("[%s] Detected a duplicated IMC (%s)! Original Setup: '%s', Provided Setup: '%s'"),
             *GetNameSafe(GetOwner()), *GetNameSafe(NewContext), *GetNameSafe(OriginalSetup.SourceData), *GetNameSafe(SetupData));

        return;
    }
    
    TArray<FProcessedBinding> Bindings;
    AddInputMappingContext(NewContext, SetupData->Priority, Bindings);

    if (!Bindings.IsEmpty())
    {
        const FProcessedInputSetup Setup(SetupData, Bindings);
        ProcessedSetups.Add(NewContext, Setup);

        UE_LOG(LogNinjaInputManagerComponent, Log, TEXT("[%s] Added Setup Data %s with %d bindings."),
            *GetNameSafe(GetOwner()), *GetNameSafe(SetupData), Bindings.Num());
    }
    else
    {
        // We don't have any bindings so this Context is pointless. Remove it so it can be added again later.
        RemoveInputMappingContext(NewContext);

        UE_LOG(LogNinjaInputManagerComponent, Warning, TEXT("[%s] Discarded Setup Data %s as it has no bindings."),
            *GetNameSafe(GetOwner()), *GetNameSafe(SetupData));
    }
}

void UNinjaInputManagerComponent::AddInputMappingContext(const UInputMappingContext* InputMappingContext, const int32 Priority, TArray<FProcessedBinding>& OutBindings)
{
    OutBindings.Reset();
    
	if (IsValid(InputComponent) && ensure(IsValid(InputMappingContext)))
	{
	    if (IsValid(InputMappingContext) && !HasInputMappingContext(InputMappingContext))
	    {
	        const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = GetEnhancedInputSubsystem(OwnerController.Get());
	        check(IsValid(Subsystem));
	        
	        Subsystem->AddMappingContext(InputMappingContext, Priority);
	
	        TArray<TObjectPtr<const UInputAction>> ActionsToBind;
	        ActionsToBind.Reserve(InputMappingContext->GetMappings().Num());
	        OutBindings.Reserve(ActionsToBind.Num());

	        // Ensure that we only process each action once, regardless of how many keys are assigned to them.
	        for (const auto& KeyMapping: InputMappingContext->GetMappings())
	        {
	            ActionsToBind.AddUnique(KeyMapping.Action);
	        }

	        // Create bindings for each unique action, for every tracked trigger and then store the Handle.
	        for (const auto InputAction : ActionsToBind)
	        {
	            const TArray<ETriggerEvent> TrackedEvents = GetDefault<UNinjaInputSettings>()->TrackedEvents;
	            for (const ETriggerEvent TriggerEvent : TrackedEvents)
	            {
	                FEnhancedInputActionHandlerInstanceSignature::TMethodPtr<ThisClass> DispatchFunction = nullptr;
	                switch (TriggerEvent)
	                {
	                case ETriggerEvent::Started:
	                    DispatchFunction = &ThisClass::DispatchStartedEvent;
	                    break;
                    case ETriggerEvent::Triggered:
                        DispatchFunction = &ThisClass::DispatchTriggeredEvent;
	                    break;
                    case ETriggerEvent::Ongoing:
	                    DispatchFunction = &ThisClass::DispatchOngoingEvent;
	                    break;
	                case ETriggerEvent::Completed:
	                    DispatchFunction = &ThisClass::DispatchCompletedEvent;
	                    break;
                    case ETriggerEvent::Canceled:
	                    DispatchFunction = &ThisClass::DispatchCancelledEvent;
	                    break;
                    default:
                        const UEnum* EnumPtr = FindObject<UEnum>(GetOuter(), TEXT("ETriggerEvent"), true);
	                    UE_LOG(LogNinjaInputManagerComponent, Warning, TEXT("[%s] Unable to handle Trigger Event '%s'."),
                            *GetNameSafe(GetOwner()), *EnumPtr->GetValueAsString(TriggerEvent));
                    }

	                if (DispatchFunction != nullptr)
	                {
	                    FEnhancedInputActionEventBinding* Handle = &InputComponent->BindAction(InputAction, TriggerEvent, this, DispatchFunction);
	                    if (Handle != nullptr)
	                    {
	                        FProcessedBinding Binding(InputAction, TriggerEvent, Handle);
	                        OutBindings.Add(Binding);
	                    }
	                }
	            }
	        }
	    }
	}
}

void UNinjaInputManagerComponent::DispatchStartedEvent(const FInputActionInstance& ActionInstance)
{
    Dispatch(ActionInstance, ETriggerEvent::Started);
}

void UNinjaInputManagerComponent::DispatchTriggeredEvent(const FInputActionInstance& ActionInstance)
{
    Dispatch(ActionInstance, ETriggerEvent::Triggered);
}

void UNinjaInputManagerComponent::DispatchOngoingEvent(const FInputActionInstance& ActionInstance)
{
    Dispatch(ActionInstance, ETriggerEvent::Ongoing);
}

void UNinjaInputManagerComponent::DispatchCompletedEvent(const FInputActionInstance& ActionInstance)
{
    Dispatch(ActionInstance, ETriggerEvent::Completed);
}

void UNinjaInputManagerComponent::DispatchCancelledEvent(const FInputActionInstance& ActionInstance)
{
    Dispatch(ActionInstance, ETriggerEvent::Canceled);
}

void UNinjaInputManagerComponent::Dispatch(const FInputActionInstance& ActionInstance, const ETriggerEvent ActualTrigger)
{
    const FInputActionValue Value = ActionInstance.GetValue();
    const TObjectPtr<const UInputAction> InputAction = ActionInstance.GetSourceAction();

    TArray<FBufferedInputCommand> CandidateCommands;
    const TObjectPtr<UActorComponent> InputBuffer = GetInputBufferComponent();
    const bool bIsUsingBuffer = IsValid(InputBuffer) && Execute_IsInputBufferOpen(InputBuffer);

    for (auto It(ProcessedSetups.CreateConstIterator()); It; ++It)
    {
        for (TObjectPtr<UNinjaInputHandler> Handler : ProcessedSetups.Find(It.Key())->SourceData->InputHandlers)
        {
            if (IsValid(Handler) && Handler->CanHandle(ActualTrigger, InputAction))
            {
                if (bIsUsingBuffer && Handler->CanBeBuffered())
                {
                    UE_LOG(LogNinjaInputManagerComponent, VeryVerbose, TEXT("[%s] Input Action %s will be saved as an Input Buffer candidate."),
                        *GetNameSafe(GetOwner()), *GetNameSafe(InputAction));
                
                    FBufferedInputCommand NewCommand(this, InputAction, Handler, Value, ActualTrigger);
                    CandidateCommands.AddUnique(NewCommand);
                }
                else
                {
                    UE_LOG(LogNinjaInputManagerComponent, VeryVerbose, TEXT("[%s] Input Action %s will trigger handler %s."),
                        *GetNameSafe(GetOwner()), *GetNameSafe(InputAction), *GetNameSafe(Handler));

                    Handler->SetWorld(GetWorld());
                    Handler->HandleInput(this, Value, ActualTrigger, InputAction);
                }
            }
        }
    }

    Execute_BufferInputCommands(InputBuffer, CandidateCommands);
}

void UNinjaInputManagerComponent::ClearInputSetup()
{
    // Iterate on the keys instead of the map directly, as each removal function invoked will modify it.
    TArray<TObjectPtr<UInputMappingContext>> Keys;
    ProcessedSetups.GetKeys(Keys);

    for (TObjectPtr<UInputMappingContext> Key : Keys)
    {
        // Avoid race conditions of the controller being changed/gone and the component is already shutting down.
        if (ProcessedSetups.Contains(Key))
        {
            RemoveInputSetupData(ProcessedSetups[Key].SourceData);
        }
    }
}

void UNinjaInputManagerComponent::RemoveInputSetupData(const UNinjaInputSetupDataAsset* SetupData)
{
    if (ensure(IsValid(SetupData)) && HasSetupData(SetupData))
    {
        // Make sure the buffered handlers from this setup won't be executed later.
        for (TObjectPtr<UNinjaInputHandler> Handler : SetupData->InputHandlers)
        {
            BufferedCommands.RemoveAll([Handler](const FBufferedInputCommand& Command)
                { return Command.Handler == Handler; });
        }
        
        RemoveInputMappingContext(SetupData->InputMappingContext);
    }
}

void UNinjaInputManagerComponent::RemoveInputMappingContext(const UInputMappingContext* InputMappingContext)
{
    if (ensure(IsValid(InputMappingContext)) && HasInputMappingContext(InputMappingContext))
    {
        if (ProcessedSetups.Contains(InputMappingContext))
        {
            FProcessedInputSetup Setup = ProcessedSetups.FindAndRemoveChecked(InputMappingContext);
            for (auto It = Setup.ProcessedBindings.CreateIterator(); It; ++It)
            {
                // Deliberately not using "IsValid()", as it may have GC flags if we're in the "EndPlay" flow.
                if (InputComponent)
                {
                    const FProcessedBinding Binding = *It;
                    InputComponent->RemoveBinding(*Binding.Handle);
                }
                
                // We still want to remove this entry, regardless of the Input Component being available or not.
                It.RemoveCurrent();
            }
        }

        const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = GetEnhancedInputSubsystem(OwnerController.Get());
        check(IsValid(Subsystem));
        Subsystem->RemoveMappingContext(InputMappingContext);

        UE_LOG(LogNinjaInputManagerComponent, Log, TEXT("[%s] Removed Input Context %s."),
            *GetNameSafe(GetOwner()), *GetNameSafe(InputMappingContext));
    }
}

// Gameplay Ability System Support ------------------------------------------------------

UAbilitySystemComponent* UNinjaInputManagerComponent::GetAbilitySystemComponent() const
{
    return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}

int32 UNinjaInputManagerComponent::SendGameplayEventToOwner(const FGameplayTag& GameplayEventTag,
    const FInputActionValue& Value, const UInputAction* InputAction, const bool bSendLocally, bool const bSendToServer) const
{
    int32 Activations = 0;
    const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController());
    
    if (IsValid(PlayerController) && ensureMsgf(GameplayEventTag.IsValid(), TEXT("The Gameplay Event Tag must be valid.")))
    {
        if ((bSendLocally && PlayerController->IsLocalController()) || (bSendToServer && GetOwner()->HasAuthority()))
        {
            // Local execution for either local client or authoritative version.
            Activations = FNinjaInputHandlerHelpers::SendGameplayEvent(this, GameplayEventTag, Value, InputAction);
        }
        
        if (PlayerController->IsLocalController() && bSendToServer && !GetOwner()->HasAuthority())
        {
            // On local client and we need to send this event to the server.
            Server_SendGameplayEventToOwner(GameplayEventTag, Value, InputAction);
        }

        if (GetOwner()->HasAuthority() && bSendLocally && !PlayerController->IsLocalController())
        {
            // On a server and we need to send this event to the client.
            Client_SendGameplayEventToOwner(GameplayEventTag, Value, InputAction);
        }
    }

    return Activations;
}

void UNinjaInputManagerComponent::Client_SendGameplayEventToOwner_Implementation(const FGameplayTag& GameplayEventTag,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    const FString Context = "Client RPC";
    FNinjaInputHandlerHelpers::SendGameplayEvent(this, GameplayEventTag, Value, InputAction, Context);
}

void UNinjaInputManagerComponent::Server_SendGameplayEventToOwner_Implementation(const FGameplayTag& GameplayEventTag,
    const FInputActionValue& Value, const UInputAction* InputAction) const
{
    const FString Context = "Server RPC";
    FNinjaInputHandlerHelpers::SendGameplayEvent(this, GameplayEventTag, Value, InputAction, Context);
}

// Support and Getter Functions ---------------------------------------------------------

bool UNinjaInputManagerComponent::HasSetupData(const UNinjaInputSetupDataAsset* SetupData) const
{
    for (auto It(ProcessedSetups.CreateConstIterator()); It; ++It)
    {
        if (It.Value().SourceData == SetupData)
        {
            return true;
        }
    }
    
    return false;
}

bool UNinjaInputManagerComponent::HasInputMappingContext(const UInputMappingContext* InputMappingContext) const
{
    check(IsValid(InputMappingContext));
	
    const UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem(OwnerController.Get());
    return IsValid(Subsystem) && Subsystem->HasMappingContext(InputMappingContext);
}

bool UNinjaInputManagerComponent::HasCompatibleInputHandler(const UInputAction* InputAction, const ETriggerEvent& TriggerEvent) const
{
    for (auto It(ProcessedSetups.CreateConstIterator()); It; ++It)
    {
        if (It.Value().SourceData->HasCompatibleInputHandler(InputAction, TriggerEvent))
        {
            return true;
        }
    }

    return false;
}

FVector UNinjaInputManagerComponent::GetForwardVector_Implementation() const
{
	FVector Reference;
	GetVectorForAxis(EAxis::X, Reference);
	return Reference;
}

FVector UNinjaInputManagerComponent::GetRightVector_Implementation() const
{
	FVector Reference;
	GetVectorForAxis(EAxis::Y, Reference);
	return Reference;
}

void UNinjaInputManagerComponent::GetVectorForAxis_Implementation(const EAxis::Type Axis, FVector& OutReference) const
{
	if (IsValid(ForwardReference))
	{
		const FRotator ReferenceYawRotation = ForwardReference->GetComponentRotation();
		OutReference = FRotationMatrix(ReferenceYawRotation).GetScaledAxis(Axis);
	}
	else if (IsValid(OwnerController))
	{
		FRotator ControlRotation = OwnerController->GetControlRotation();
		ControlRotation.Roll = 0.f;
		ControlRotation.Pitch = 0.f;
		OutReference = FRotationMatrix(ControlRotation).GetScaledAxis(Axis);
	}
	else switch (Axis)
	{
		case EAxis::X:
			OutReference = GetOwner()->GetActorForwardVector();
			break;
		case EAxis::Y:
			OutReference = GetOwner()->GetActorRightVector();
			break;
		case EAxis::Z:
			OutReference = GetOwner()->GetActorUpVector();
			break;
		default:
			checkNoEntry();
	}
}

UEnhancedInputLocalPlayerSubsystem* UNinjaInputManagerComponent::GetEnhancedInputSubsystem(AController* Controller) const
{
    if (IsValid(Controller))
    {
        const APlayerController* PlayerController = Cast<APlayerController>(Controller);
        if (IsValid(PlayerController))
        {
            const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
            if (IsValid(LocalPlayer))
            {
                return LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
            }
        }
    }

    return nullptr;
}

FVector UNinjaInputManagerComponent::GetLastInputVector() const
{
    const TObjectPtr<APawn> PawnOwner = GetPawn();
    if (IsValid(PawnOwner))
    {
        if (PawnOwner->Implements<ULastInputProviderInterface>())
        {
            return ILastInputProviderInterface::Execute_GetLastInputVector(PawnOwner);
        }

        const TObjectPtr<UPawnMovementComponent> PawnMovement = PawnOwner->GetMovementComponent();
        if (IsValid(PawnOwner->GetMovementComponent()))
        {
            return PawnMovement->GetLastInputVector();
        }
    }

    return FVector::ZeroVector;
}

APawn* UNinjaInputManagerComponent::GetPawn() const
{
	TObjectPtr<APawn> Pawn = nullptr;
	
	if (GetOwner()->IsA(APawn::StaticClass()))
	{
		Pawn = Cast<APawn>(GetOwner());
	}
	else if (GetOwner()->IsA(AController::StaticClass()))
	{
		const AController* Controller = Cast<AController>(GetOwner());
		Pawn = Controller->GetPawn();
	}
	else if (GetOwner()->IsA(APlayerState::StaticClass()))
	{
		const APlayerState* PlayerState = Cast<APlayerState>(GetOwner());
		Pawn = PlayerState->GetPawn();
	}

	ensureAlwaysMsgf(IsValid(Pawn), TEXT("Unable to retrieve the owning Pawn."));
	return Pawn;
}

AController* UNinjaInputManagerComponent::GetController() const
{
	TObjectPtr<AController> Controller = nullptr;

	if (GetOwner()->IsA(APawn::StaticClass()))
	{
		const TObjectPtr<const APawn> Pawn = Cast<APawn>(GetOwner());
		Controller = Pawn->GetController();
	}
	else if (GetOwner()->IsA(AController::StaticClass()))
	{
		Controller = Cast<AController>(GetOwner());
	}
	else if (GetOwner()->IsA(APlayerState::StaticClass()))
	{
		const TObjectPtr<APlayerState> PlayerState = Cast<APlayerState>(GetOwner());
		Controller = PlayerState->GetOwningController();
	}

	ensureAlwaysMsgf(IsValid(Controller), TEXT("Unable to retrieve the owning Controller."));
	return Controller;
}

bool UNinjaInputManagerComponent::IsLocallyControlled() const
{
    const TObjectPtr<const APawn> MyPawn = GetPawn();
    return IsValid(MyPawn) && MyPawn->IsLocallyControlled();
}