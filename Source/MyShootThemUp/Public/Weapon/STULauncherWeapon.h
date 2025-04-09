// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
#include "STULauncherWeapon.generated.h"

class ASTUProjectile;

UCLASS()
class MYSHOOTTHEMUP_API ASTULauncherWeapon : public ASTUBaseWeapon
{
	GENERATED_BODY()
	public:
		virtual void StartFire() override;
		virtual void MakeShot() override;
    protected:
		UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Weapon")
		TSubclassOf<ASTUProjectile> ProjectileClass;
};
