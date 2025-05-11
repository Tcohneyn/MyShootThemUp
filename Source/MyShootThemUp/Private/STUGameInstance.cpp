// Shoot Them Up Game,All Right Reserved.


#include "STUGameInstance.h"
#include "Sound/STUSoundFunctionLib.h"

void USTUGameInstance::ToggleVolume() 
{
    USTUSoundFunctionLib::ToggleSoundClassVolume(MasterSoundClass);
}
