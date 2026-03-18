#include "Game/NBGameStateBase.h"
#include <Kismet/GameplayStatics.h>
#include "Player/NBPlayerController.h"

void ANBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
    if (HasAuthority() == false)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (IsValid(PC) == true)
        {
            ANBPlayerController* NBPC = Cast <ANBPlayerController>(PC);
            if (IsValid(NBPC) == true)
            {
                FString NotificationString = InNameString + TEXT(" 입장");
                NBPC->PrintChatMessageString(NotificationString);
            }
        }
    }
}

void ANBGameStateBase::MulticastRPCUpdateRemainingTime_Implementation(int32 InRemainingTime)
{
    if (HasAuthority() == true) return; 

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    ANBPlayerController* NBPC = Cast<ANBPlayerController>(PC);
    if (IsValid(NBPC) == true)
    {
        if (InRemainingTime == 0)
        {
            NBPC->TimeCountText = FText::FromString(
                FString::Printf(TEXT(""))
            );
            return;
        }
        NBPC->TimeCountText = FText::FromString(
            FString::Printf(TEXT("종료까지 %ds"), InRemainingTime)
        );
    }
}
