#include "HealingItem.h"
#include "TaskCharacter.h"

AHealingItem::AHealingItem()
{
    HealAmount = 20.0f;
    ItemType = "Healing";
}

void AHealingItem::ActivateItem(TObjectPtr<AActor> Activator)
{
    Super::ActivateItem(Activator);
    
    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (ATaskCharacter* PlayerCharacter = Cast<ATaskCharacter>(Activator))
        {
            PlayerCharacter->AddHealth(HealAmount);
        }
        DestroyItem();
    }
}
