// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "NinjaInputBufferComponent.h"

#include "InputAction.h"
#include "NinjaInputFunctionLibrary.h"
#include "NinjaInputHandler.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY(LogNinjaInputBufferComponent);

UNinjaInputBufferComponent::UNinjaInputBufferComponent()
{
    InputBufferMode = EInputBufferMode::LastCommand;
    bUsingInputBuffer = false;
}

bool UNinjaInputBufferComponent::IsInputBufferEnabled_Implementation() const
{
    return InputBufferMode != EInputBufferMode::Disabled;
}

bool UNinjaInputBufferComponent::IsInputBufferOpen_Implementation() const
{
    return bUsingInputBuffer;
}

void UNinjaInputBufferComponent::BufferInputCommands_Implementation(TArray<FBufferedInputCommand>& InputCommandsForAction)
{
    if (!InputCommandsForAction.IsEmpty() && CanAddToBuffer())
    {
        BufferedCommands.Empty();
        BufferedCommands = InputCommandsForAction;

        UE_LOG(LogNinjaInputBufferComponent, Verbose, TEXT("[%s] Action %s added %d Handlers to the Input Buffer."),
            *GetNameSafe(GetOwner()), *GetNameSafe(InputCommandsForAction[0].InputAction), BufferedCommands.Num());
    }
}

void UNinjaInputBufferComponent::OpenInputBuffer_Implementation()
{
    if (!Execute_IsInputBufferOpen(this))
    {
        BufferedCommands.Empty();
        bUsingInputBuffer = true;
    }
}

void UNinjaInputBufferComponent::CloseInputBuffer_Implementation(const bool bCancelled)
{
    if (Execute_IsInputBufferOpen(this))
    {
        bUsingInputBuffer = false;
        
        for (auto It(BufferedCommands.CreateIterator()); It; ++It)
        {
            // Only execute commands if this buffer has not been cancelled.
            if (!bCancelled)
            {
                const FBufferedInputCommand Command = *It;
                if (Command.IsValid())
                {
                    UE_LOG(LogNinjaInputBufferComponent, Verbose, TEXT("[%s] Releasing Input Action %s and Handler %s from buffer."),
                        *GetNameSafe(GetOwner()), *GetNameSafe(Command.InputAction), *GetNameSafe(Command.Handler));
            
                    Command.Execute();
                }
            }

            // The command must be removed, regardless of the buffer finishing or being cancelled.
            It.RemoveCurrent();
        }
    }
}

UActorComponent* UNinjaInputBufferComponent::GetInputBufferComponent()
{
    // If this buffer is enabled, then there's no need to perform the look-up.
    if (Execute_IsInputBufferEnabled(this))
    {
        return Cast<UActorComponent>(this);
    }

    // Try to find another buffer that's currently active.
    return UNinjaInputFunctionLibrary::GetInputBufferComponent(GetOwner());
}

bool UNinjaInputBufferComponent::CanAddToBuffer() const
{
    return InputBufferMode == EInputBufferMode::LastCommand
        || InputBufferMode == EInputBufferMode::FirstCommand && BufferedCommands.IsEmpty();
}
