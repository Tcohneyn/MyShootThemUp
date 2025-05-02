// Shoot Them Up Game,All Right Reserved.


#include "AI/STUAICharacter.h"
#include "AI/STUAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/STUAIWeaponComponent.h"
#include "BrainComponent.h"

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


}

void ASTUAICharacter::OnDeath() {
    Super::OnDeath();

    const auto STUController = Cast<AAIController>(GetController());
    if (STUController && STUController->BrainComponent)
    {
        STUController->BrainComponent->Cleanup();
    }
}

