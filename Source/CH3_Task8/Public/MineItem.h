#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

class UWidgetComponent;

UCLASS()
class CH3_TASK8_API AMineItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AMineItem();
	
	UFUNCTION(BlueprintCallable, Category = "Mine")
	void ExplosionDelayMultiplier(float Value);
	UFUNCTION(BlueprintCallable, Category = "Mine")
	void ExplosionDamageMultiplier(float Value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> OverheadWidget;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	TObjectPtr<USphereComponent> ExplosionCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	TObjectPtr<UParticleSystem> ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
	float ExplosionDamage;
	
	bool bHasExploded;
	FTimerHandle ExplosionTimerHandle;
	FTimerHandle UpdateUITimerHandle;

	virtual void BeginPlay() override;
	virtual void ActivateItem(TObjectPtr<AActor> Activator) override;
	
	void Explode();
	void UpdateOverheadUI();
};
