// Ninja Bear Studio Inc. 2023, all rights reserved.
#pragma once

class FDataValidationContext;
class UNinjaInputSetupDataAsset;

/** Provides validation logic for the Input Setup Data Asset. */
struct FNinjaInputSetupDataValidator
{
    
#if WITH_EDITOR

    /** Validates the provided data asset and adds any messages to the validation context. */
    static EDataValidationResult ValidateInputSetupData(const UNinjaInputSetupDataAsset* SetupData, FDataValidationContext& Context);

    /** Determines if the Handlers should be matched against Input Actions from the Context. */
    static bool ShouldMatchHandlers();

    /** Determines the proper result to return, based on the final context and current settings. */
    static EDataValidationResult DetermineResult(const FDataValidationContext& Context);

#endif
};
