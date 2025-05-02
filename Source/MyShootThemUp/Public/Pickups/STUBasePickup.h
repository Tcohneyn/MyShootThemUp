// Shoot Them Up Game,All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUBasePickup.generated.h"

class USphereComponent;
class ASTUBaseWeapon;

UCLASS()
class MYSHOOTTHEMUP_API ASTUBasePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTUBasePickup();

protected:
    UPROPERTY(VisibleAnywhere,Category = "Pickup")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
    float RespawnTime = 5.0f;

	//UPROPERTY(VisibleAnywhere, Category = "Pickup")
 //   bool CouldBeTakenTest = true;

	virtual void BeginPlay() override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    

public:	
	virtual void Tick(float DeltaTime) override;
    virtual TSubclassOf<ASTUBaseWeapon> GetWeaponType() const { return nullptr; }
    bool CouldBeTaken() const;

private:
	float RotationYaw = 0.0f;
    FTimerHandle RespawnTimerHandle;

    virtual bool GivePickupTo(APawn* PlayerOwner);
    void PickupWasTaken();
    void Respawn();
	void GenerateRotationYaw();
};
