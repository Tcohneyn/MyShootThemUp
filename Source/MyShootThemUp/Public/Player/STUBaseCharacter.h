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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<UCameraComponent> CameraComponet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Componets")
    TObjectPtr<USpringArmComponent> SpringArmComponent;

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

	UPROPERTY()
	TSubclassOf<UInputComponent> InputComponentClass;
	// Called when the game starts or when spawned
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

};
