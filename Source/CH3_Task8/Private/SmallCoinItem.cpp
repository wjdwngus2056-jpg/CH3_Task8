#include "SmallCoinItem.h"

ASmallCoinItem::ASmallCoinItem()
{
    PointValue = 10;
    ItemType = "SmallCoin";
}

void ASmallCoinItem::ActivateItem(TObjectPtr<AActor> Activator)
{
    Super::ActivateItem(Activator);
}
