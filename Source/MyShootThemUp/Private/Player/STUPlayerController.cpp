// Shoot Them Up Game,All Right Reserved.

#include "Player/STUPlayerController.h"
#include "Components/STURespawnComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "STUGameModeBase.h"
#include "STUGameInstance.h"


ASTUPlayerController::ASTUPlayerController()
{
    RespawnComponent = CreateDefaultSubobject<USTURespawnComponent>("STURespawnComponent");
    //InputComponentClass = UEnhancedInputComponent::StaticClass();
}

void ASTUPlayerController::BeginPlay() 
{
    Super::BeginPlay();
    if (GetWorld())
    {
        if (const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode()))
        {
            GameMode->OnMatchStateChanged.AddUObject(this, &ASTUPlayerController::OnMatchStateChanged);
        }
    }
}

void ASTUPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    SetupInputComponent();  // 重新绑定输入
    OnNewPawn.Broadcast(InPawn);
}

void ASTUPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
     //ASTUPlayerCharacter* PlayerCharacter = Cast<ASTUPlayerCharacter>(GetPawn());
     //check(PlayerCharacter)
     //if (!PlayerCharacter) return;
    //UEnhancedInputLocalPlayerSubsystem* Subsystem =
    //    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    //Subsystem->AddMappingContext(DefaultMapping, 1);
     if (InputType == EInputType::RawInput)
    {
        if (!InputComponent) return;

        InputComponent->BindAction("PauseGame", IE_Pressed, this, &ASTUPlayerController::OnPauseGame);        
        InputComponent->BindAction("Mute", IE_Pressed, this, &ASTUPlayerController::OnMuteSound);
    }
    else if (InputType == EInputType::EnhancedInput)
    {
        UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
        if (!EnhancedInput) return;
        EnhancedInput->BindAction(PauseGameAction, ETriggerEvent::Started, this, &ASTUPlayerController::OnPauseGame);        
        EnhancedInput->BindAction(MuteAction, ETriggerEvent::Started, this, &ASTUPlayerController::OnMuteSound);
    }
}

void ASTUPlayerController::OnPauseGame()
{
    if (!GetWorld()||!GetWorld()->GetAuthGameMode()) return;
    GetWorld()->GetAuthGameMode()->SetPause(this);

}

void ASTUPlayerController::OnMatchStateChanged(ESTUMatchState State) 
{
    if (State == ESTUMatchState::InProgress)
    {
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
    }
    else
    {
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
    }
}

void ASTUPlayerController::OnMuteSound()
{
    if (!GetWorld()) return;

    const auto STUGameInstance = GetWorld()->GetGameInstance<USTUGameInstance>();
    if (!STUGameInstance) return;
    STUGameInstance->ToggleVolume();
}

//EInputType ASTUPlayerController::SetInputType(EInputType InputType)
//{
//    return EInputType();
//}
