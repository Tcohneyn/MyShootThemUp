#pragma once
#include "STUCoreTypes.generated.h"

class ASTUBaseWeapon;

DECLARE_MULTICAST_DELEGATE(FOnClipEmptySignature);//BaseWeapon

USTRUCT(BlueprintType)//BaseWeapon
struct FAmmoData
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    int32 Bullets;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "!Infintie"))
    int32 Clips;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    bool Infintie;
};

USTRUCT(BlueprintType)//WeaponComponent
struct FWeaponData
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ASTUBaseWeapon> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* ReloadAnimMontage;
};

//HealthComponent
DECLARE_MULTICAST_DELEGATE(FOnDeath);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);