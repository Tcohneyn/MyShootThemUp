// Shoot Them Up Game,All Right Reserved.


#include "Weapon/STULauncherWeapon.h"
#include "WEAPON/STUProjectile.h"
#include "Kismet/GameplayStatics.h"

void ASTULauncherWeapon::StartFire() 
{
	MakeShot();
}

void ASTULauncherWeapon::MakeShot() 
{
    if (!GetWorld()||IsAmmoEmpty()) return;
    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd)) return;
    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);
    const FVector EndPoint=HitResult.bBlockingHit? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - GetMuzzleLocation()).GetSafeNormal();
    const FTransform SpawnTransform (FRotator::ZeroRotator,GetMuzzleLocation());
    ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(GetOwner());
        Projectile->FinishSpawning(SpawnTransform);
    }
    DecreaseAmmo();
    SpawnMuzzleFX();
}
