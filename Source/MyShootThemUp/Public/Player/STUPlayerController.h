// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STUPlayerController.generated.h"

class USTURespawnComponent;

UCLASS()
class MYSHOOTTHEMUP_API ASTUPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    ASTUPlayerController();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    TObjectPtr<USTURespawnComponent> RespawnComponent;

    virtual void OnPossess(APawn* InPawn) override;
};
