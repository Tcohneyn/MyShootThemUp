// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/STUBasePickup.h"
#include "STUAmmoPickup.generated.h"

class ASTUBaseWeapon;

UCLASS()
class MYSHOOTTHEMUP_API ASTUAmmoPickup : public ASTUBasePickup
{
    GENERATED_BODY()
public:
    virtual TSubclassOf<ASTUBaseWeapon> GetWeaponType() const override { return WeaponType; }

protected:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Pickup",meta=(ClampMin="1.0",ClampMax="10.0"))
    int32 ClipsAmount = 10;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Pickup")
    TSubclassOf<ASTUBaseWeapon> WeaponType;


private:
    virtual bool GivePickupTo(APawn* PlayerOwner) override;
};
