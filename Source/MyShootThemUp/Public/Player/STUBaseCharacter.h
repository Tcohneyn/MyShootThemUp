// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "STUBaseCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class USTUHealthComponent;
class UTextRenderComponent;
class UAnimMontage;
class USTUWeaponComponent;

UENUM(BlueprintType)
enum class EInputType:uint8
{
	RawInput,
	EnhancedInput
};

UCLASS()
class MYSHOOTTHEMUP_API ASTUBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASTUBaseCharacter(const FObjectInitializer& ObjInit);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<USpringArmComponent> SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<USTUHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
	TObjectPtr<UTextRenderComponent> HealthTextComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
	TObjectPtr<USTUWeaponComponent> WeaponComponent;
	//USTUWeaponComponent* WeaponComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
    EInputType InputType = EInputType::EnhancedInput;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<UInputMappingContext> DefaultMapping;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<UInputAction> RunAction;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> FireAction;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> WeaponAction;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FVector2D LandDamageVelocity=FVector2D(900.f,1200.f);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FVector2D LandDamage=FVector2D(10.f,100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float LifeSpanOnDeath = 5.0f;

	UPROPERTY()
	TSubclassOf<UInputComponent> InputComponentClass;


	virtual void BeginPlay() override;



public:	
	UFUNCTION(BlueprintCallable, Category = "Movement")
    bool IsRunning() const;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetMovementDirection() const;

private:
    bool WantsToRun = false;
	bool IsMovingForward = false;
    FVector2D MovementVector = FVector2D::ZeroVector;
    FVector2D LookVector = FVector2D::ZeroVector;
	//Move组件
    void MoveForward(float Amount);
	void MoveRight(float Amount);
    void Move(const FInputActionValue& Value);
	//Look组件
    void Look(const FInputActionValue& Value);
	//Jump组件
    void Jumping(const FInputActionValue& Value);
	//Running组件
	void OnStartRunning();
	void OnStopRunning();
	void Run(const FInputActionValue& Value);

	void OnDeath();
	void OnHealthChanged(float Health,float HealthDelta);
	UFUNCTION()
    void OnGroundLand(const FHitResult& Hit);


};
