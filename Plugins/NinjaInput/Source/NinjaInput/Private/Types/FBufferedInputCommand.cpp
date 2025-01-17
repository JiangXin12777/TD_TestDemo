// Ninja Bear Studio Inc. 2023, all rights reserved.
#include "Types/FBufferedInputCommand.h"

#include "NinjaInputHandler.h"

bool FBufferedInputCommand::IsValid() const
{
    return Source != nullptr && InputAction != nullptr && Handler != nullptr && TriggerEvent != ETriggerEvent::None;
}

void FBufferedInputCommand::Execute() const
{
    if (IsValid())
    {
        Handler->HandleInput(Source, Value, TriggerEvent, InputAction);
    }
}
