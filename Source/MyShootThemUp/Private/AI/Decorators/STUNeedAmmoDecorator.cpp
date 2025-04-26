// Shoot Them Up Game,All Right Reserved.


#include "AI/Decorators/STUNeedAmmoDecorator.h"
#include "AIController.h"
#include "Components/STUWeaponComponent.h"
#include "STUUtils.h"

USTUNeedAmmoDecorator::USTUNeedAmmoDecorator() {
    NodeName = "Need Ammo";
}

bool USTUNeedAmmoDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const auto Contorller = OwnerComp.GetAIOwner();
    if (!Contorller) return false;
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(Contorller->GetPawn());
    if (!WeaponComponent) return false;

    return WeaponComponent->NeedAmmo(WeaponType);
}
