// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "STUHealthPercentDecorator.generated.h"

/**
 * 
 */
UCLASS()
class MYSHOOTTHEMUP_API USTUHealthPercentDecorator : public UBTDecorator
{
	GENERATED_BODY()

	public:
    USTUHealthPercentDecorator();
	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float HealthPercent = 0.6f;

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
