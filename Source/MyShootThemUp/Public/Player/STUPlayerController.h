// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STUPlayerCharacter.h"
#include "STUPlayerController.generated.h"

class USTURespawnComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class MYSHOOTTHEMUP_API ASTUPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    ASTUPlayerController();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    TObjectPtr<USTURespawnComponent> RespawnComponent;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<UInputAction> PauseGameAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
    EInputType InputType = EInputType::EnhancedInput;

    // UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
    // EInputType InputType = EInputType::EnhancedInput;

    // UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    // TObjectPtr<UInputMappingContext> DefaultMapping;

    // UPROPERTY()
    // TSubclassOf<UInputComponent> InputComponentClass;

    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void SetupInputComponent() override;

private:
    void OnPauseGame();
    void OnMatchStateChanged(ESTUMatchState State);
    // EInputType SetInputType(EInputType InputType);
};
