// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/STUBaseCharacter.h"
#include "STUPlayerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EInputType : uint8
{
    RawInput,
    EnhancedInput
};

UCLASS()
class MYSHOOTTHEMUP_API ASTUPlayerCharacter : public ASTUBaseCharacter
{
    GENERATED_BODY()
public:
    ASTUPlayerCharacter(const FObjectInitializer& ObjInit);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<UCameraComponent> CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<USpringArmComponent> SpringArmComponent;  

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<USphereComponent> CameraCollisionComponent;

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

    UPROPERTY()
    TSubclassOf<UInputComponent> InputComponentClass;

    virtual void BeginPlay() override;
    virtual void OnDeath() override;

public:
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual bool IsRunning() const override;

private:
    bool WantsToRun = false;
    bool IsMovingForward = false;
    FVector2D MovementVector = FVector2D::ZeroVector;
    FVector2D LookVector = FVector2D::ZeroVector;

    // Move组件
    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void Move(const FInputActionValue& Value);
    // Look组件
    void Look(const FInputActionValue& Value);
    // Jump组件
    void Jumping(const FInputActionValue& Value);
    // Running组件
    void Run(const FInputActionValue& Value);
    void OnStartRunning();
    void OnStopRunning();

    UFUNCTION()
    void OnCameraCollisionBeginOverlap(UPrimitiveComponent*
        OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void CheckCamereOverlap();

};
