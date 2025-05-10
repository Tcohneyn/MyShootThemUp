// Shoot Them Up Game,All Right Reserved.


#include "AI/STUAICharacter.h"
#include "AI/STUAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/STUAIWeaponComponent.h"
#include "BrainComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/STUHealthBarWidget.h"
#include "Components/STUHealthComponent.h"
// 构造函数，初始化父类
ASTUAICharacter::ASTUAICharacter(const FObjectInitializer& ObjInit): Super(ObjInit.SetDefaultSubobjectClass<USTUAIWeaponComponent>("WeaponComponet"))
{
    // 设置自动处理AI的方式
    AutoPossessAI = EAutoPossessAI::Disabled;
    // 设置AI控制器的类
    AIControllerClass = ASTUAIController::StaticClass();
    bUseControllerRotationYaw = false;
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bOrientRotationToMovement = true;
        GetCharacterMovement()->RotationRate = FRotator(0.f, 200.f, 0.f);
    }

    HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("HealthWidgetComponent");
    HealthWidgetComponent->SetupAttachment(GetRootComponent());
    HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    HealthWidgetComponent->SetDrawAtDesiredSize(true);
}

void ASTUAICharacter::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);
    UpdateHealthWidgetVisibility();
}

void ASTUAICharacter::BeginPlay() 
{
    Super::BeginPlay(); 
    check(HealthWidgetComponent);
}

void ASTUAICharacter::OnHealthChanged(float Health, float HealthDelta)
{
    Super::OnHealthChanged(Health, HealthDelta);

    const auto HealthBarWidget = Cast<USTUHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject());
    if (!HealthBarWidget) return;
    HealthBarWidget->SetHealthPercent(HealthComponent->GetHealthPercent());
}

void ASTUAICharacter::OnDeath()
{
    Super::OnDeath();

    const auto STUController = Cast<AAIController>(GetController());
    if (STUController && STUController->BrainComponent)
    {
        STUController->BrainComponent->Cleanup();
    }
}

void ASTUAICharacter::UpdateHealthWidgetVisibility() 
{
    if (!GetWorld() ||                              //
        !GetWorld()->GetFirstPlayerController() ||  //
        !GetWorld()->GetFirstPlayerController()->GetPawnOrSpectator())
        return;

    const auto PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawnOrSpectator()->GetActorLocation();
    const auto Distance = FVector::Distance(PlayerLocation, GetActorLocation());
    HealthWidgetComponent->SetVisibility(Distance < HealthVisibilityDistance, true);
}

