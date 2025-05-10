// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/STUBaseWidget.h"
#include "STUPauseWidget.generated.h"

class UButton;

UCLASS()
class MYSHOOTTHEMUP_API USTUPauseWidget : public USTUBaseWidget
{
	GENERATED_BODY()
	public:
    virtual void NativeOnInitialized() override;

    protected:
        UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> ClearPauseButton;

    private:
        UFUNCTION()
        void OnClearPause();
};
