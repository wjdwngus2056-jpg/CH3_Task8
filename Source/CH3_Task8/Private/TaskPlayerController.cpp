#include "TaskPlayerController.h"
#include "TaskGameState.h"
#include "TaskGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

ATaskPlayerController::ATaskPlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    LookAction(nullptr),
    JumpAction(nullptr),
    SprintAction(nullptr),
    HUDWidgetClass(nullptr),
    HUDWidgetInstance(nullptr),
    MainMenuWidgetClass(nullptr),
    MainMenuWidgetInstance(nullptr)
{
}

void ATaskPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (TObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer())
    {
        if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }
    
    FString CurrentMapName = GetWorld()->GetName();
    if (CurrentMapName.Contains("MenuLevel"))
    {
        ShowMainMenu(false);
    }
    
}

UUserWidget* ATaskPlayerController::GetHUDWidget() const
{
    return HUDWidgetInstance;
}

void ATaskPlayerController::ShowMainMenu(bool bIsRestart)
{
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }
    
    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }
    
    if (MainMenuWidgetClass)
    {
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        if (MainMenuWidgetInstance)
        {
            MainMenuWidgetInstance->AddToViewport();
            
            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }
        
        if (TObjectPtr<UTextBlock> ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
        {
            if (bIsRestart)
            {
                ButtonText->SetText(FText::FromString(TEXT("Restart")));
            }
            else
            {
                ButtonText->SetText(FText::FromString(TEXT("Start")));
            }
        }
        
        if (bIsRestart)
        {
            TObjectPtr<UFunction> PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
            if (PlayAnimFunc)
            {
                MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
            }
            
            if (TObjectPtr<UTextBlock> TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(("TotalScoreText"))))
            {
                if (TObjectPtr<UTaskGameInstance> TaskGameInstance = Cast<UTaskGameInstance>(UGameplayStatics::GetGameInstance(this)))
                {
                    TotalScoreText->SetText(FText::FromString(
                        FString::Printf(TEXT("Total Score : %d"), TaskGameInstance->TotalScore)
                    ));
                }
            }
        }
    }
}

void ATaskPlayerController::ShowGameHUD()
{
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }
    
    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }
    
    if (HUDWidgetClass)
    {
        HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidgetInstance)
        {
            HUDWidgetInstance->AddToViewport();
            
            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());
            
            TObjectPtr<ATaskGameState> TaskGameState = GetWorld() ? GetWorld()->GetGameState<ATaskGameState>() : nullptr;
            if (TaskGameState)
            {
                TaskGameState->UpdateHUD();
            }
        }
    }
}


void ATaskPlayerController::StartGame()
{
    if (TObjectPtr<UTaskGameInstance> TaskGameInstance = Cast<UTaskGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        TaskGameInstance->CurrentLevelIndex = 0;
        TaskGameInstance->TotalScore = 0;
    }
    
    UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
    SetPause(false);
}
