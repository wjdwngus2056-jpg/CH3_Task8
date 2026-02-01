#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "LargeCoinItem.generated.h"

UCLASS()
class CH3_TASK8_API ALargeCoinItem : public ACoinItem
{
	GENERATED_BODY()
	
public:
	ALargeCoinItem();

	virtual void ActivateItem(TObjectPtr<AActor> Activator) override;
};
