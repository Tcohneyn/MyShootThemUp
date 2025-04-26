// Shoot Them Up Game,All Right Reserved.


#include "Pickups/STUBasePickup.h"
#include "Components/SphereComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePickup, All, All);

ASTUBasePickup::ASTUBasePickup()
{

	PrimaryActorTick.bCanEverTick = true;
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    CollisionComponent->InitSphereRadius(60.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    SetRootComponent(CollisionComponent);
}


void ASTUBasePickup::BeginPlay()
{
	Super::BeginPlay();
    check(CollisionComponent);
    GenerateRotationYaw();
}

void ASTUBasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    AddActorLocalRotation(FRotator(0.0f, RotationYaw,0.0f));
}

bool ASTUBasePickup::CouldBeTaken() const
{
   return !GetWorldTimerManager().IsTimerActive(RespawnTimerHandle);
    //return CouldBeTakenTest;
}


void ASTUBasePickup::NotifyActorBeginOverlap(AActor* OtherActor) 
{
    Super::NotifyActorBeginOverlap(OtherActor);
    const auto Pawn=Cast<APawn>(OtherActor);
    if (GivePickupTo(Pawn))
    {
    PickupWasTaken();
    }
 /*   UE_LOG(LogBasePickup, Display, TEXT("Pickup was taken"));*/
}

bool ASTUBasePickup::GivePickupTo(APawn* PlayerPawn)
{
    return false;
}

void ASTUBasePickup::PickupWasTaken()
{
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    if (GetRootComponent())
    {
        GetRootComponent()->SetVisibility(false, true);
    }

    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASTUBasePickup::Respawn, RespawnTime);
}

void ASTUBasePickup::Respawn() 
{
    GenerateRotationYaw();
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    if (GetRootComponent())
    {
        GetRootComponent()->SetVisibility(true,true);
    }
}

void ASTUBasePickup::GenerateRotationYaw() 
{
    const auto Direction = FMath::RandBool()? 1.0f : -1.0f;
    RotationYaw = FMath::FRandRange(1.0f, 2.0f) * Direction;
}
