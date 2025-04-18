// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class USTUWeaponFXComponent;

UCLASS()
class MYSHOOTTHEMUP_API ASTUProjectile : public AActor
{
    GENERATED_BODY()

public:
    ASTUProjectile();

    void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TObjectPtr<UProjectileMovementComponent> MovementComponent;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    TObjectPtr<USTUWeaponFXComponent> WeaponFXComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float DamageRadius = 200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float DamageAmount = 50.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    bool DoFullDamage = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float LifeSeconds = 5.0f;
    virtual void BeginPlay() override;

private:
    FVector ShotDirection;
    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);
    AController* GetController() const;
};
