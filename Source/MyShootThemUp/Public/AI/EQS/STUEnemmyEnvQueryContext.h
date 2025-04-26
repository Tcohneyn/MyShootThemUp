// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "STUEnemmyEnvQueryContext.generated.h"

/**
 * 
 */
UCLASS()
class MYSHOOTTHEMUP_API USTUEnemmyEnvQueryContext : public UEnvQueryContext
{
	GENERATED_BODY()
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    FName EnemyActorKeyName = "EnemyActor";
};
