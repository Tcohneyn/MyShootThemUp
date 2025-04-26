// Shoot Them Up Game,All Right Reserved.


#include "AI/Decorators/STUHealthPercentDecorator.h"
#include "AIController.h"
#include "Components/STUHealthComponent.h"
#include "STUUtils.h"

USTUHealthPercentDecorator::USTUHealthPercentDecorator() 
{
	NodeName = "Health Percent";
}

bool USTUHealthPercentDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const auto Contorller = OwnerComp.GetAIOwner();
    if (!Contorller) return false;

    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(Contorller->GetPawn());
    if (!HealthComponent || HealthComponent->IsDead()) return false;
    return HealthComponent->GetHealthPercent() <= HealthPercent;

}
