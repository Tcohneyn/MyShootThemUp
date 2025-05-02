// Shoot Them Up Game,All Right Reserved.

#include "Player/STUBaseCharacter.h"
#include "GameFramework/Controller.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"


DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All);

// Sets default values
ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // 创建一个HealthComponent
    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>(TEXT("HealthComponet"));

    //// 创建一个HealthTextComponent，并将其附加到角色的根组件上
    //HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthTextComponet"));
    //HealthTextComponent->SetupAttachment(GetRootComponent());
    //// 设置HealthTextComponent的可见性
    //HealthTextComponent->SetOwnerNoSee(true);

    // 创建一个WeaponComponent
    WeaponComponent = CreateDefaultSubobject<USTUWeaponComponent>(TEXT("WeaponComponet"));

}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 检查HealthComponent、HealthTextComponent、GetCharacterMovement()和GetMesh()是否为空
    check(HealthComponent);
    // check(HealthTextComponent);
    check(GetCharacterMovement());
    ensure(GetMesh());

    // 调用OnHealthChanged函数，更新HealthTextComponent的文本
    OnHealthChanged(HealthComponent->GetHealth(), 0.0f);
    // 绑定HealthComponent的OnDeath和OnHealthChanged事件到相应的函数
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);
    // 绑定LandedDelegate到OnGroundLand函数
    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLand);
}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 获取HealthComponent的Health值
    const auto Health = HealthComponent->GetHealth();
}

// Called to bind functionality to input
bool ASTUBaseCharacter::IsRunning() const
{
    return false;
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    // 如果角色的速度为零，则返回0.0f
    if (GetVelocity().IsZero()) return 0.0f;
    // 获取角色的速度方向
    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    // 计算角色速度方向与角色前向方向的夹角
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    // 计算角色速度方向与角色前向方向的叉积
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
    // 将夹角转换为度数
    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
    // 如果叉积为零，则返回夹角，否则返回夹角乘以叉积的符号
    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::SetPlayerColor(const FLinearColor& Color)
{
    const auto MaterialInstance = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
    if (!MaterialInstance) return;

    MaterialInstance->SetVectorParameterValue(MaterialColorName, Color);
}

void ASTUBaseCharacter::OnDeath()
{
    UE_LOG(BaseCharacterLog, Display, TEXT("Player %s is dead"), *GetName());

    // PlayAnimMontage(DeathAnimMontage);

    // 禁用角色的移动
    GetCharacterMovement()->DisableMovement();

    // 设置角色的生命周期
    SetLifeSpan(LifeSpanOnDeath);

    // 设置角色的CapsuleComponent的碰撞响应
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    // 停止角色的射击
    WeaponComponent->StopFire();

    // 设置角色的Mesh的碰撞模式和物理模拟
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

void ASTUBaseCharacter::OnHealthChanged(float Health, float HealthDelta)
{
    //// 更新HealthTextComponent的文本
    //HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
}

void ASTUBaseCharacter::OnGroundLand(const FHitResult& Hit)
{
    /*    const auto FallVelocityZ = -GetCharacterMovement()->Velocity.Z; */
    // 获取角色的速度在Z轴上的分量
    const auto FallVelocityZ = -GetVelocity().Z;
    UE_LOG(BaseCharacterLog, Display, TEXT("On landed:velocity %f"), FallVelocityZ);
    // 如果角色的速度小于一定值，则不受伤
    if (FallVelocityZ < LandDamageVelocity.X) return;  // 跳跃时速度小于一定值，不受伤
    // 计算最终伤害
    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandDamageVelocity, LandDamage, FallVelocityZ);
    UE_LOG(BaseCharacterLog, Display, TEXT("FinalDamage %f"), FinalDamage);
    // 伤害角色
    TakeDamage(FinalDamage, FDamageEvent(), nullptr, nullptr);
}