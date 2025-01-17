// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "Data/NinjaInputSetupDataAsset.h"

#include "NinjaInputHandler.h"
#include "Data/NinjaInputSetupDataValidator.h"

UNinjaInputSetupDataAsset::UNinjaInputSetupDataAsset()
{
    Priority = 0;
}

FPrimaryAssetId UNinjaInputSetupDataAsset::GetPrimaryAssetId() const
{
    const FPrimaryAssetType BaseAssetType = TEXT("InputSetup");
    return FPrimaryAssetId(BaseAssetType, GetFName());
}

bool UNinjaInputSetupDataAsset::HasCompatibleInputHandler(const UInputAction* InputAction,
    const ETriggerEvent& TriggerEvent) const
{
    return InputHandlers.ContainsByPredicate([InputAction, TriggerEvent](const UNinjaInputHandler* Handler)
        { return IsValid(Handler) && Handler->CanHandle(TriggerEvent, InputAction); });
}

#if WITH_EDITOR
EDataValidationResult UNinjaInputSetupDataAsset::IsDataValid(FDataValidationContext& Context)
{
    return FNinjaInputSetupDataValidator::ValidateInputSetupData(this, Context);
}
#endif
