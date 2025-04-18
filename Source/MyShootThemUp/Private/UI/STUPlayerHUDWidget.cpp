// Shoot Them Up Game,All Right Reserved.


#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "STUUtils.h"

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (!HealthComponent) return 0.0f;
    return HealthComponent->GetHealthPercent();
}

bool USTUPlayerHUDWidget::GetCurrentWeaponUIData(FWeaponUIData& UIData) const
{
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent) return false;
    return WeaponComponent->GetCurrentWeaponUIData(UIData);
}

bool USTUPlayerHUDWidget::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const
{
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent) return false;
    return WeaponComponent->GetCurrentWeaponAmmoData(AmmoData);
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    return HealthComponent&&!HealthComponent->IsDead();
}

bool USTUPlayerHUDWidget::IsPlayerSpectating() const
{
    const auto Controller = GetOwningPlayer();
    return Controller && Controller->GetStateName()==NAME_Spectating;
}

bool USTUPlayerHUDWidget::Initialize()
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (HealthComponent)
    {
        HealthComponent->OnHealthChanged.AddUObject(this, &USTUPlayerHUDWidget::OnHealthChanged);
    }
    return Super::Initialize();
}

void USTUPlayerHUDWidget::OnHealthChanged(float Health,float HealthDelta) 
{
    if (HealthDelta<0.0f&&Health<=50.0f)
    {
        OnTakeDamage();
    }
}

//TObjectPtr<USTUWeaponComponent> USTUPlayerHUDWidget::GetWeaponComponent() const
//{
//    const auto Player = GetOwningPlayerPawn();
//    if (!Player) return nullptr;
//
//    const auto Component = Player->GetComponentByClass(USTUWeaponComponent::StaticClass());
//    const auto WeaponComponent = Cast<USTUWeaponComponent>(Component);
//    return WeaponComponent;
//}
//
//TObjectPtr<USTUHealthComponent> USTUPlayerHUDWidget::GetHealthComponent() const
//{
//    const auto Player = GetOwningPlayerPawn();
//    if (!Player) return nullptr;
//
//    const auto Component = Player->GetComponentByClass(USTUHealthComponent::StaticClass());
//    const auto HealthComponent = Cast<USTUHealthComponent>(Component);
//    return HealthComponent;
//}
