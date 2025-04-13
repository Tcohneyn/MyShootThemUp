// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/STUBasePickup.h"
#include "STUHealthPickup.generated.h"


UCLASS()
class MYSHOOTTHEMUP_API ASTUHealthPickup : public ASTUBasePickup
{
	GENERATED_BODY()
	protected:
    UPROPERTY(EditAnywhere, Category = "Pickup", meta = (ClampMin = "1.0", ClampMax = "100.0"))
	float HealthAmount=100.0f;
	private:
    virtual bool GivePickupTo(APawn* PlayerOwner) override;
	
};
