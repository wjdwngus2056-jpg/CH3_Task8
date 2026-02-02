#include "TaskGameState.h"
#include "TaskGameInstance.h"
#include "TaskPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "TaskCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"

ATaskGameState::ATaskGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentWaveIndex = 0;
	CurrentLevelIndex = 0;
	MaxWave = 3;
	MaxLevels = 3;
	WaveDuration = 10.0f;
	LevelDuration = WaveDuration * MaxWave;
}

void ATaskGameState::BeginPlay()
{
	Super::BeginPlay();
	
	StartLevel();
	
	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ATaskGameState::UpdateHUD,
		0.1f,
		true);
}

int32 ATaskGameState::GetScore() const
{
	return Score;
}

void ATaskGameState::AddScore(int32 Amount)
{
	if (TObjectPtr<UGameInstance> GameInstance = GetGameInstance())
	{
		TObjectPtr<UTaskGameInstance> TaskGameInstance = Cast<UTaskGameInstance>(GameInstance);
		if (TaskGameInstance)
		{
			TaskGameInstance->AddToScore(Amount);
		}
	}
}


void ATaskGameState::StartWave()
{
	CurrentWaveIndex = FMath::Clamp(CurrentWaveIndex+1, 0, MaxWave);
	UE_LOG(LogTemp, Warning, TEXT("CurrentWave : %d"), CurrentWaveIndex);
	
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	WaveDuration = WaveDuration * CurrentWaveIndex;
	const int32 ItemToSpawn = 10 * (CurrentLevelIndex+1) * CurrentWaveIndex;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			TObjectPtr<ASpawnVolume> SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				TObjectPtr<AActor> SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}
	
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ATaskGameState::OnWaveTimeUp,
		WaveDuration,
		true
	);
}

void ATaskGameState::StartLevel()
{
	if (TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (TObjectPtr<ATaskPlayerController> TaskPlayerController = Cast<ATaskPlayerController>(PlayerController))
		{
			TaskPlayerController->ShowGameHUD();
		}
	}
	
	if (TObjectPtr<UGameInstance> GameInstance = GetGameInstance())
	{
		TObjectPtr<UTaskGameInstance> TaskGameInstance = Cast<UTaskGameInstance>(GameInstance);
		if (TaskGameInstance)
		{
			CurrentLevelIndex = TaskGameInstance->CurrentLevelIndex;
		}
	}
	
	StartWave();
	
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ATaskGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);
}

void ATaskGameState::OnWaveTimeUp()
{
	EndWave();
}

void ATaskGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ATaskGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected : %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount && CurrentWaveIndex == MaxWave)
	{
		EndLevel();
	}
}

void ATaskGameState::EndWave()
{
	if (CurrentWaveIndex > MaxWave) return;

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	
	StartWave();
}

void ATaskGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	
	if (TObjectPtr<UGameInstance> GameInstance = GetGameInstance())
	{
		TObjectPtr<UTaskGameInstance> TaskGameInstance = Cast<UTaskGameInstance>(GameInstance);
		if (TaskGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			TaskGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}
	
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ATaskGameState::OnGameOver()
{
	if (TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (TObjectPtr<ATaskPlayerController> TaskPlayerController = Cast<ATaskPlayerController>(PlayerController))
		{
			TaskPlayerController->SetPause(true);
			TaskPlayerController->ShowMainMenu(true);
		}
	}
}

void ATaskGameState::UpdateHUD()
{
	if (TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (TObjectPtr<ATaskPlayerController> TaskPlayerController = Cast<ATaskPlayerController>(PlayerController))
		{
			if (TObjectPtr<UUserWidget> HUDWidget = TaskPlayerController->GetHUDWidget())
			{
				if (TObjectPtr<UProgressBar> TimeBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("LevelTimer"))))
				{
					float LevelTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeBar->SetPercent(LevelTime / LevelDuration);
				}
				
				if (TObjectPtr<UProgressBar> WaveBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("WaveTimer"))))
				{
					float WaveTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					WaveBar->SetPercent(WaveTime / WaveDuration);
				}
				
				if (TObjectPtr<UProgressBar> HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HP"))))
				{
					if (TObjectPtr<AActor> Character = TaskPlayerController->GetCharacter())
					{
						TObjectPtr<ATaskCharacter> Player = Cast<ATaskCharacter>(Character);
						if (Player)
						{
							HPBar->SetPercent(Player->GetHealth() / Player->GetMaxHealth());
						}
					}
				}
				
				if (TObjectPtr<UTextBlock> HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("HPText"))))
				{
					if (TObjectPtr<AActor> Character = TaskPlayerController->GetCharacter())
					{
						TObjectPtr<ATaskCharacter> Player = Cast<ATaskCharacter>(Character);
						if (Player)
						{
							HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Player->GetHealth(), Player->GetMaxHealth())));
						}
					}
				}
				
				
				if (TObjectPtr<UTextBlock> ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (TObjectPtr<UGameInstance> GameInstance = GetGameInstance())
					{
						TObjectPtr<UTaskGameInstance> TaskGameInstance = Cast<UTaskGameInstance>(GameInstance);
						if (TaskGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Socre : %d"), TaskGameInstance->TotalScore)));
						}
					}
				}
				
				if (TObjectPtr<UTextBlock> CoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Coin"))))
				{
					CoinText->SetText(FText::FromString(FString::Printf(TEXT("Coin : %d / %d"), CollectedCoinCount, SpawnedCoinCount)));
				}
				
				if (TObjectPtr<UTextBlock> LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level : %d"), CurrentLevelIndex + 1)));
				}
				
				if (TObjectPtr<UTextBlock> WaveIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave : %d"),CurrentWaveIndex)));
				}
			}
		}
	}
}
