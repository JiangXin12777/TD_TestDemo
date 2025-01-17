// Ninja Bear Studio Inc. 2022, all rights reserved.
#include "NinjaInputEditor.h"

#include "AssetTypeActions/AssetTypeAction_NinjaInputHandler.h"
#include "AssetTypeActions/AssetTypeAction_NinjaInputSetup.h"

#define LOCTEXT_NAMESPACE "FNinjaInputEditorModule"

DEFINE_LOG_CATEGORY(LogNinjaInputEditor);
IMPLEMENT_MODULE(FNinjaInputEditorModule, NinjaInputEditor)

EAssetTypeCategories::Type FNinjaInputEditorModule::InputAssetsCategory;

void FNinjaInputEditorModule::StartupModule()
{
    // Register our assets.
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    InputAssetsCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Input")), LOCTEXT("InputAssetsCategory", "Input"));
    {
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FAssetTypeActions_NinjaInputHandler));
        RegisterAssetTypeActions(AssetTools, MakeShareable(new FAssetTypeActions_NinjaInputSetup));
    }
}

void FNinjaInputEditorModule::ShutdownModule()
{
    // Unregister input assets.
    if (const FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
    {
        for (TSharedPtr<IAssetTypeActions>& AssetAction : CreatedAssetTypeActions)
        {
            AssetToolsModule->Get().UnregisterAssetTypeActions(AssetAction.ToSharedRef());
        }
    }
    
    CreatedAssetTypeActions.Empty();
}
#undef LOCTEXT_NAMESPACE