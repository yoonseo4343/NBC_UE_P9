#include "Player/NBPlayerController.h"
#include "UI/NBChatInput.h"
#include <Kismet/KismetSystemLibrary.h>
#include "NumberBaseball.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/NBGameModeBase.h"
#include "NBPlayerState.h"
#include <Net/UnrealNetwork.h>

ANBPlayerController::ANBPlayerController()
{
    bReplicates = true;
}

void ANBPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() == false)
    {
        return;
    }
    FInputModeUIOnly InputModeUIOnly;
    SetInputMode(InputModeUIOnly);

    if (IsValid(ChatInputWidgetClass) == true)
    {
        ChatInputWidgetInstance = CreateWidget<UNBChatInput>(this, ChatInputWidgetClass);
        if (IsValid(ChatInputWidgetInstance) == true)
        {
            ChatInputWidgetInstance->AddToViewport();
        }
    }

    if (IsValid(NotificationTextWidgetClass) == true)
    {
        NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
        if (IsValid(NotificationTextWidgetInstance) == true)
        {
            NotificationTextWidgetInstance->AddToViewport();
        }
    }
}

void ANBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
    ChatMessageString = InChatMessageString;

    //PrintChatMessageString(InChatMessageString);
    if (IsLocalController() == true)
    {
        //ServerRPCPrintChatMessageString(InChatMessageString); // 서버 전달

        ANBPlayerState* NBPS = GetPlayerState<ANBPlayerState>();
        if (IsValid(NBPS) == true)
        {
            FString CombinedMessageString = NBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

            ServerRPCPrintChatMessageString(CombinedMessageString);
        }
    }
}

void ANBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
    //UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);

    //FString NetModeString = NBFunctionLibrary::GetNetModeString(this);
    //FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
    //NBFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);

    NBFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ANBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, NotificationText);
    DOREPLIFETIME(ThisClass, TimeCountText);
}

void ANBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
    /*for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
    {
        ANBPlayerController* NBPlayerController = *It;
        if (IsValid(NBPlayerController) == true)
        {
            NBPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
        }
    }*/

    AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
    if (IsValid(GM) == true)
    {
        ANBGameModeBase* NBGM = Cast<ANBGameModeBase>(GM);
        if (IsValid(NBGM) == true)
        {
            NBGM->PrintChatMessageString(this, InChatMessageString);
        }
    }
}

void ANBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
    PrintChatMessageString(InChatMessageString);
}
