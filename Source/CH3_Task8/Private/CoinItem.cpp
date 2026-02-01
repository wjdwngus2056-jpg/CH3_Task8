#include "CoinItem.h"
#include "Engine/World.h"
#include "TaskGameState.h"

ACoinItem::ACoinItem()
{
    PointValue = 0;
    ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(TObjectPtr<AActor> Activator)
{
    Super::ActivateItem(Activator);
    
    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (UWorld* World = GetWorld())
        {
            if (ATaskGameState* GameState = World->GetGameState<ATaskGameState>())
            {
                GameState->AddScore(PointValue);
                GameState->OnCoinCollected();
            }
        }
        DestroyItem();
    }
}
