#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TaskGameInstance.generated.h"

UCLASS()
class CH3_TASK8_API UTaskGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UTaskGameInstance();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;
	
	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
};
