#include "Player/NBPlayerState.h"
#include "Net/UnrealNetwork.h"

ANBPlayerState::ANBPlayerState()
    : PlayerNameString(TEXT("None"))
    , CurrentGuessCount(0)
    , MaxGuessCount(5)
{
    bReplicates = true;
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // 프로퍼티 리플리케이션
    DOREPLIFETIME(ThisClass, PlayerNameString);
    DOREPLIFETIME(ThisClass, CurrentGuessCount);
    DOREPLIFETIME(ThisClass, MaxGuessCount);

}

FString ANBPlayerState::GetPlayerInfoString()
{
    //FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
    FString PlayerInfoString = PlayerNameString;

    return PlayerInfoString;
}

int32 ANBPlayerState::GetCurrentGuessCount()
{
    return CurrentGuessCount;
}

int32 ANBPlayerState::GetMaxGuessCount()
{
    return MaxGuessCount;
}
