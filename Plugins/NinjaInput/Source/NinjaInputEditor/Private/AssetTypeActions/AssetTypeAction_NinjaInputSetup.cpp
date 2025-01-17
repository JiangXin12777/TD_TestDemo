// Ninja Bear Studio Inc. 2022, all rights reserved.
#include "AssetTypeActions/AssetTypeAction_NinjaInputSetup.h"

#include "NinjaInputEditor.h"
#include "Data/NinjaInputSetupDataAsset.h"

FText FAssetTypeActions_NinjaInputSetup::GetName() const
{
    return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_InputSetup_Title", "Ninja Input Setup");
}

uint32 FAssetTypeActions_NinjaInputSetup::GetCategories()
{
    return FNinjaInputEditorModule::GetInputAssetsCategory();
}

FColor FAssetTypeActions_NinjaInputSetup::GetTypeColor() const
{
    return FColor(106, 117, 94);
}

FText FAssetTypeActions_NinjaInputSetup::GetAssetDescription(const FAssetData& AssetData) const
{
    return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_InputSetup_Description", 
        "Maps multiple Input Action Contexts and Actions to their appropriate Input Handlers.");
}

UClass* FAssetTypeActions_NinjaInputSetup::GetSupportedClass() const
{
    return UNinjaInputSetupDataAsset::StaticClass();
}
