// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "Data/NinjaInputSetupDataValidator.h"

#if WITH_EDITOR

#include "InputMappingContext.h"
#include "NinjaInputHandler.h"
#include "NinjaInputSettings.h"
#include "Data/NinjaInputSetupDataAsset.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "InputSetupNamespace"
EDataValidationResult FNinjaInputSetupDataValidator::ValidateInputSetupData(const UNinjaInputSetupDataAsset* SetupData, FDataValidationContext& Context)
{
    bool bHasContext = true;
    if (!IsValid(SetupData->InputMappingContext))
    {
        const FText Message = NSLOCTEXT("InputSetupNamespace", "MissingMappingContext", "Invalid Input Mapping Context!");
        Context.AddError(Message);
        bHasContext = false;
    }

    bool bHasHandlers = true;
    if (SetupData->InputHandlers.IsEmpty())
    {
        const FText Message = NSLOCTEXT("InputSetupNamespace", "MissingInputHandlers", "No Input Handlers provided!");
        Context.AddError(Message);
        bHasHandlers = false;
    }

    if (bHasContext && bHasHandlers && ShouldMatchHandlers())
    {
        TArray<TObjectPtr<const UInputAction>> ActionsToCheck;
        for (const auto& KeyMapping: SetupData->InputMappingContext->GetMappings())
        {
            ActionsToCheck.AddUnique(KeyMapping.Action);
        }
        
        for (auto It(ActionsToCheck.CreateIterator()); It; ++It)
        {
            for (const TObjectPtr<const UNinjaInputHandler> Handler : SetupData->InputHandlers)
            {
                if (IsValid(Handler))
                {
                	if (Handler->HasAnyInputActions())
                	{
                		if (Handler->HandlesAction(*It))
                		{
                			It.RemoveCurrent();                       
                		}  
                	}
                    else
                    {
                    	FFormatNamedArguments Args;
                    	Args.Add(TEXT("Handler"), FText::FromString(GetNameSafe(Handler)));
            
                    	const FText Message = FText::Format(NSLOCTEXT("InputSetupNamespace", "HandlerWithoutAction", "No Input Actions assigned to Handler {Handler}!"), Args);
                    	Context.AddError(Message);
                    }

                	if (!Handler->HasAnyTriggerEvents())
                	{
                		FFormatNamedArguments Args;
                		Args.Add(TEXT("Handler"), FText::FromString(GetNameSafe(Handler)));
            
                		const FText Message = FText::Format(NSLOCTEXT("InputSetupNamespace", "HandlerWithoutTrigger", "No Trigger Events assigned to Handler {Handler}!"), Args);
                		Context.AddError(Message);
                	}
                }
                else
                {
                    const FText Message = NSLOCTEXT("InputSetupNamespace", "InvalidInputHandler", "Empty entry in the Handler List!");
                	Context.AddError(Message);
                }
            }
        }

        for (TObjectPtr<const UInputAction> Action : ActionsToCheck)
        {
            FFormatNamedArguments Args;
            Args.Add(TEXT("Action"), FText::FromString(GetNameSafe(Action)));
            
            const FText Message = FText::Format(NSLOCTEXT("InputSetupNamespace", "UnmatchedAction", "No Input Handlers for Action {Action}!"), Args);
        	Context.AddError(Message);
        }
    }
    
    return DetermineResult(Context);
}

bool FNinjaInputSetupDataValidator::ShouldMatchHandlers()
{
    const TObjectPtr<const UNinjaInputSettings> Settings = GetDefault<UNinjaInputSettings>();
    return Settings->bEnableDataValidation && Settings->bMatchHandlersWithContext;
}

EDataValidationResult FNinjaInputSetupDataValidator::DetermineResult(const FDataValidationContext& Context)
{
    const TObjectPtr<const UNinjaInputSettings> Settings = GetDefault<UNinjaInputSettings>();
    if (Settings->bEnableDataValidation && Context.GetNumErrors() > 0 || Context.GetNumWarnings() > 0)
    {
        return EDataValidationResult::Invalid;
    }
    
    return EDataValidationResult::Valid;
}

#undef LOCTEXT_NAMESPACE
#endif