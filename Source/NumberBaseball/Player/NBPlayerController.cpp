#include "Player/NBPlayerController.h"
#include "UI/NBChatInput.h"
#include <Kismet/KismetSystemLibrary.h>

void ANBPlayerController::BeginPlay()
{
    Super::BeginPlay();

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
}

void ANBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
    ChatMessageString = InChatMessageString;

    PrintChatMessageString(InChatMessageString);
}

void ANBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
    UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);
}
