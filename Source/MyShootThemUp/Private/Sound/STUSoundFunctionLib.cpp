// Shoot Them Up Game,All Right Reserved.


#include "Sound/STUSoundFunctionLib.h"
#include "Sound/SoundClass.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUSoundFunctionLib, All, All);

void USTUSoundFunctionLib::SetSoundClassVolume(USoundClass* SoundClass, float Volume)
{
    if (!SoundClass) return;
    SoundClass->Properties.Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
    UE_LOG(LogSTUSoundFunctionLib,Display,TEXT("Sound class volume was changed: %s = %f"),*SoundClass->GetName(),SoundClass->Properties.Volume);
}

void USTUSoundFunctionLib::ToggleSoundClassVolume(USoundClass* SoundClass) 
{
    if (!SoundClass) return;

    const auto NextVolume = SoundClass->Properties.Volume > 0.0f ? 0.0f : 1.0f;
    SetSoundClassVolume(SoundClass, NextVolume);

}
