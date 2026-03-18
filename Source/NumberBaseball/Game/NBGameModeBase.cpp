#include "Game/NBGameModeBase.h"
#include "NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "EngineUtils.h"
#include "Player/NBPlayerState.h"

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
    Super::OnPostLogin(NewPlayer);

	ANBPlayerController* NBPC = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(NBPC) == true)
	{
		NBPC->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(NBPC);

		ANBPlayerState* NBPS = NBPC->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			// 플레이어 이름 번호 
			NBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ANBGameStateBase* NBGS = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGS) == true)
		{
			NBGS->MulticastRPCBroadcastLoginMessage(NBPS->PlayerNameString);
		}
	}
}

#pragma region NumberBaseball
FString ANBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ANBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ANBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}
void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SecretNumberString = GenerateSecretNumber();
}
void ANBGameModeBase::PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		// 숫자 야구 입력이 들어 왔을 경우
		FString JudgeResultString;
		// 기회 남았는지 판단
		if (!IncreaseGuessCount(InChattingPlayerController))
		{
			// 기회가 없다
			JudgeResultString = TEXT("No Chance...");
		}
		else
		{
			JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
			int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
			JudgeGame(InChattingPlayerController, StrikeCount);
		}
		FString ChanceString = Chance(InChattingPlayerController);
		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPC = *It;
			if (IsValid(NBPC) == true)
			{
				FString CombineMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString + TEXT(" ") + ChanceString;
				NBPC->ClientRPCPrintChatMessageString(CombineMessageString);
			}
		}
	}
	else
	{
		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPC = *It;
			if (IsValid(NBPC) == true)
			{
				NBPC->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}
bool ANBGameModeBase::IncreaseGuessCount(ANBPlayerController* InChattingPlayerController)
{
	ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState <ANBPlayerState>();
	if (IsValid(NBPS) == true)
	{
		if (NBPS->CurrentGuessCount < NBPS->MaxGuessCount)
			NBPS->CurrentGuessCount++;
		else
			return false;
	}
	return true;
}
FString ANBGameModeBase::Chance(ANBPlayerController* InChattingPlayerController)
{
	ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState <ANBPlayerState>();
	if (IsValid(NBPS) == true)
	{
		return  TEXT("(") + FString::FromInt(NBPS->CurrentGuessCount) + TEXT(" / ") + FString::FromInt(NBPS->MaxGuessCount) + TEXT(")");
	}
	return TEXT("");
}
void ANBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& NBPC : AllPlayerControllers)
	{
		ANBPlayerState* NBPS = NBPC->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			NBPS->CurrentGuessCount = 0;
		}
	}
}
void ANBGameModeBase::JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		for (const auto& NBPC : AllPlayerControllers)
		{
			if (IsValid(NBPS) == true)
			{
				FString CombinedMessageString = NBPS->PlayerNameString + TEXT(" has won the game!");
				NBPC->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& NBPC : AllPlayerControllers)
		{
			ANBPlayerState* NBPS = NBPC->GetPlayerState<ANBPlayerState>();
			if (IsValid(NBPS) == true)
			{
				if (NBPS->CurrentGuessCount < NBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}
		if (true == bIsDraw)
		{
			for (const auto& NBPC : AllPlayerControllers)
			{
				NBPC->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}
#pragma endregion