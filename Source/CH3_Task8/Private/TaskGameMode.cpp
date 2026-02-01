#include "TaskGameMode.h"
#include "TaskCharacter.h"
#include "TaskPlayerController.h"
#include "TaskGameState.h"

ATaskGameMode::ATaskGameMode()
{
    DefaultPawnClass = ATaskCharacter::StaticClass();
    PlayerControllerClass = ATaskPlayerController::StaticClass();
    GameStateClass = ATaskGameState::StaticClass();
}
