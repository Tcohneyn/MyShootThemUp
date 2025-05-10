// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "STUBaseCharacter.generated.h"


class USTUHealthComponent;
class UTextRenderComponent;
class UAnimMontage;
class USTUWeaponComponent;



UCLASS()
class MYSHOOTTHEMUP_API ASTUBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ASTUBaseCharacter(const FObjectInitializer& ObjInit);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<USTUHealthComponent> HealthComponent;

    //UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    //TObjectPtr<UTextRenderComponent> HealthTextComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<USTUWeaponComponent> WeaponComponent;
    // USTUWeaponComponent* WeaponComponent;



    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> DeathAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FVector2D LandDamageVelocity = FVector2D(900.f, 1200.f);

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FVector2D LandDamage = FVector2D(10.f, 100.f);

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float LifeSpanOnDeath = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FName MaterialColorName = "Paint Color";



    virtual void BeginPlay() override;
    virtual void OnDeath();
    virtual void OnHealthChanged(float Health, float HealthDelta);

public:
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual bool IsRunning() const;
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    float GetMovementDirection() const;

    void SetPlayerColor(const FLinearColor& Color);

private:
    UFUNCTION()
    void OnGroundLand(const FHitResult& Hit);
};
