// Shoot Them Up Game,All Right Reserved.


#include "Weapon/STURifleWeapon.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ASTURifleWeapon::ASTURifleWeapon() 
{
    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>(TEXT("WeaponFXComponent"));
}
void ASTURifleWeapon::BeginPlay() 
{
    Super::BeginPlay();
    ensure(WeaponFXComponent);
}
void ASTURifleWeapon::StartFire()
{
    //Super::StartFire();
    // UE_LOG(LogBaseWeapon,Display, TEXT("Fire"));
    InitMuzzleFX();
    GetWorld()->GetTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}
void ASTURifleWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    SetMuzzleFXVisibility(false);
}


void ASTURifleWeapon::MakeShot()
{
    UE_LOG(LogTemp, Display, TEXT("MakeShot"));
    if (!GetWorld() || IsAmmoEmpty())
    {
        StopFire();
        return;
    }
    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))
    {
        StopFire();
        return;
    }
    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    FVector TraceFXEnd = TraceEnd;
    if (HitResult.bBlockingHit)
    {
        TraceEnd=HitResult.ImpactPoint;
        MakeDamage(HitResult);

       // DrawDebugLine(GetWorld(), GetMuzzleLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.f, 0, 3.f);
       // DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 24, FColor::Red, false, 5.f);
        // UE_LOG(LogBaseWeapon, Display, TEXT("Bone: %s"), *HitResult.BoneName.ToString());
        WeaponFXComponent->PlayImpactFX(HitResult);
    }
    SpawnTraceFX(GetMuzzleLocation(), TraceEnd);
    //else
    //{
    //    DrawDebugLine(GetWorld(), GetMuzzleLocation(), TraceEnd, FColor::Red, false, 3.f, 0, 3.f);
    //}
    DecreaseAmmo();
}

bool ASTURifleWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;
    TraceStart = ViewLocation;
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);                       // SocketTransform.GetLocation();
    const FVector ShootDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRad);  // SocketTransform.GetRotation().GetForwardVector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}

void ASTURifleWeapon::MakeDamage(const FHitResult& HitResult)
{
    const auto DamageActor = HitResult.GetActor();
    if (!DamageActor) return;
    DamageActor->TakeDamage(DamageAmount, FDamageEvent(), GetPlayerController(), this);
}

void ASTURifleWeapon::InitMuzzleFX() 
{
    if (!MuzzleFXComponent)
    {
        MuzzleFXComponent = SpawnMuzzleFX();
    }
    SetMuzzleFXVisibility(true);
}

void ASTURifleWeapon::SetMuzzleFXVisibility(bool Visible) 
{
    if (MuzzleFXComponent)
    {
        MuzzleFXComponent->SetPaused(!Visible);
        MuzzleFXComponent->SetVisibility(Visible, true);
    }
}

void ASTURifleWeapon::SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd) 
{
    const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
    }
}
