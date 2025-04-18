// Shoot Them Up Game,All Right Reserved.

#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

USTUWeaponFXComponent::USTUWeaponFXComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponFXComponent::PlayImpactFX(const FHitResult& Hit)
{
    auto ImpactData = DefaultImpactData;
    if (Hit.PhysMaterial.IsValid())
    {
        const auto PhyMat = Hit.PhysMaterial.Get();
        if (ImpactDataMap.Contains(PhyMat))
        {
            ImpactData = ImpactDataMap[PhyMat];
        }
    }

    // Niagara
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),  //
        ImpactData.NiagaraEffect,                            //
        Hit.ImpactPoint,                                        //
        Hit.ImpactNormal.Rotation());
    // Decal
    auto DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),  //
        ImpactData.DecalData.Material,               //
        ImpactData.DecalData.Size,                   //
        Hit.ImpactPoint,                                //
        Hit.ImpactNormal.Rotation());
    if (DecalComponent)
    {
        DecalComponent->SetFadeOut(ImpactData.DecalData.LifeTime,ImpactData.DecalData.FadeOutTime);
    }

}
