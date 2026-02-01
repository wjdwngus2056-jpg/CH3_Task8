#include "LargeCoinItem.h"

ALargeCoinItem::ALargeCoinItem()
{
    PointValue = 50;
    ItemType = "LargeCoin";
}

void ALargeCoinItem::ActivateItem(TObjectPtr<AActor> Activator)
{
    Super::ActivateItem(Activator);
}
