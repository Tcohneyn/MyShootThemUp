// Shoot Them Up Game,All Right Reserved。


#include "Weapon/STURifleWeapon.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ASTURifleWeapon::ASTURifleWeapon() 
{
    // 创建默认的武器FX组件
    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>(TEXT("WeaponFXComponent"));
}
void ASTURifleWeapon::BeginPlay() 
{
    Super::BeginPlay();
    // 确保武器FX组件存在
    ensure(WeaponFXComponent);
}
void ASTURifleWeapon::StartFire()
{
    //Super::StartFire();
    // UE_LOG(LogBaseWeapon,Display, TEXT("Fire"));
    // 初始化枪口FX
    InitMuzzleFX();
    // 设置定时器，每隔一段时间发射一次子弹
    GetWorld()->GetTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    // 发射一次子弹
    MakeShot();
}
void ASTURifleWeapon::StopFire()
{
    // 清除定时器
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    // 设置枪口FX不可见
    SetMuzzleFXVisibility(false);
}


void ASTURifleWeapon::MakeShot()
{
    UE_LOG(LogTemp, Display, TEXT("MakeShot"));
    // 如果没有世界或者弹药为空，停止发射
    if (!GetWorld() || IsAmmoEmpty())
    {
        StopFire();
        return;
    }
    FVector TraceStart, TraceEnd;
    // 获取射击数据
    if (!GetTraceData(TraceStart, TraceEnd))
    {
        StopFire();
        return;
    }
    FHitResult HitResult;
    // 进行射击
    MakeHit(HitResult, TraceStart, TraceEnd);

    FVector TraceFXEnd = TraceEnd;
    // 如果击中目标，进行伤害计算
    if (HitResult.bBlockingHit)
    {
        TraceEnd=HitResult.ImpactPoint;
        MakeDamage(HitResult);

       // DrawDebugLine(GetWorld(), GetMuzzleLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.f, 0, 3.f);
       // DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 24, FColor::Red, false, 5.f);
        // UE_LOG(LogBaseWeapon, Display, TEXT("Bone: %s"), *HitResult.BoneName.ToString());
        // 播放击中FX
        WeaponFXComponent->PlayImpactFX(HitResult);
    }
    // 生成射击FX
    SpawnTraceFX(GetMuzzleLocation(), TraceEnd);
    //else
    //{
    //    DrawDebugLine(GetWorld(), GetMuzzleLocation(), TraceEnd, FColor::Red, false, 3.f, 0, 3.f);
    //}
    // 减少弹药
    DecreaseAmmo();
}

bool ASTURifleWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    // 获取玩家的视角位置和旋转
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;
    TraceStart = ViewLocation;
    // 计算射击方向的随机角度
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);                       // SocketTransform.GetLocation();
    const FVector ShootDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRad);  // SocketTransform.GetRotation().GetForwardVector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}

void ASTURifleWeapon::MakeDamage(const FHitResult& HitResult)
{
    // 获取击中目标的Actor
    const auto DamageActor = HitResult.GetActor();
    if (!DamageActor) return;
    // 对目标进行伤害计算
    DamageActor->TakeDamage(DamageAmount, FDamageEvent(), GetPlayerController(), this);
}

void ASTURifleWeapon::InitMuzzleFX() 
{
    // 如果枪口FX组件不存在，生成一个
    if (!MuzzleFXComponent)
    {
        MuzzleFXComponent = SpawnMuzzleFX();
    }
    // 设置枪口FX可见
    SetMuzzleFXVisibility(true);
}

void ASTURifleWeapon::SetMuzzleFXVisibility(bool Visible) 
{
    // 设置枪口FX的可见性
    if (MuzzleFXComponent)
    {
        MuzzleFXComponent->SetPaused(!Visible);
        MuzzleFXComponent->SetVisibility(Visible, true);
    }
}

void ASTURifleWeapon::SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd) 
{
    // 生成射击FX
    const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
    }
}