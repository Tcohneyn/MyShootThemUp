// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "STUFindEnemyService.generated.h"

/**
 *
 */
UCLASS()
class MYSHOOTTHEMUP_API USTUFindEnemyService : public UBTService
{
    GENERATED_BODY()
public:
    USTUFindEnemyService();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI") 
    FBlackboardKeySelector EnemyActorKey;

    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
