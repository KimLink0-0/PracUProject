// Fill out your copyright notice in the Description page of Project Settings.


#include "PracGameState.h"
#include "PracSpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "PracCoinItem.h"
#include "PracGameInstance.h"
#include "PracPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

APracGameState::APracGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 10.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
}

void APracGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&APracGameState::UpdateHUD,
		0.1f,
		true);
}

int32 APracGameState::GetScore() const
{
	return Score;
}

void APracGameState::AddScore(int32 Amount)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		auto* PracGameInstance = Cast<UPracGameInstance>(GameInstance);
		if (PracGameInstance)
		{
			PracGameInstance->AddToScore(Amount);
		}
	}
}

void APracGameState::StartLevel()
{
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (CurrentLevelName != FString("MenuLevel"))
	{
		StartWave();
	}
}

void APracGameState::StartWave()
{
	WaveCount++;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		auto* PracPlayerController = Cast<APracPlayerController>(PlayerController);
		if (PracPlayerController)
		{
			PracPlayerController->ShowGameHUD();
		}
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		auto* PracGameInstance = Cast<UPracGameInstance>(GameInstance);
		if (PracGameInstance)
		{
			CurrentLevelIndex = PracGameInstance->CurrentLevelIndex;
		}
	}
	
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APracSpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawnCount = 15*WaveCount;

	for (int32 i = 0; i < ItemToSpawnCount; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			auto* SpawnVolume = Cast<APracSpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();

				if (SpawnedActor && SpawnedActor->IsA(APracCoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(
	WaveTimerHandle,
	this,
	&APracGameState::OnWaveTimeUP,
	WaveDuration,
	false);
}

void APracGameState::EndWave()
{
	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APracBaseItem::StaticClass(), FoundItems);

	for (int i = 0; i < FoundItems.Num(); i++)
	{
		FoundItems[i]->Destroy();
	}

	// 현재 진행중인 웨이브 카운트가 3보다 크거나 같을 때, EndWave 가 호출되면 종료
	if (WaveCount >= 3)
	{
		EndLevel();
	}
	else
	{
		StartWave();
	}
}

void APracGameState::OnWaveTimeUP()
{
	EndWave();
}

void APracGameState::OnLevelTimeUp()
{
	EndLevel();
}

void APracGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning,
		TEXT("Coins Collected: %d"), CollectedCoinCount)

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

void APracGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	WaveCount = 0;
	CurrentLevelIndex++;

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		auto* PracGameInstance = Cast<UPracGameInstance>(GameInstance);
		if (PracGameInstance)
		{
			PracGameInstance->AddToScore(GetScore());
			PracGameInstance->CurrentLevelIndex = CurrentLevelIndex;
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

void APracGameState::UpdateHUD()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		auto* PracPlayerController = Cast<APracPlayerController>(PlayerController);
		if (PracPlayerController)
		{
			UUserWidget* HUDWidget = PracPlayerController->GetHUDWidget();
			if (HUDWidget)
			{
				// 시간 위젯 텍스트 설정
				UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time")));
				if (TimeText)
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT
						("Time : %.1f"), RemainingTime)));
				}

				// 점수 위젯 텍스트 설정
				UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score")));
				if (ScoreText)
				{
					const UGameInstance* GameInstance = GetGameInstance();
					if (GameInstance)
					{
						const auto* PracGameInstance = Cast<UPracGameInstance>(GameInstance);
						if (PracGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT
								("Score : %d"), PracGameInstance->TotalScore)));
						}
					}
				}
				
				// 레벨 위젯 텍스트 설정
				UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level")));
				if (LevelText)
				{
					LevelText->SetText(FText::FromString(FString::Printf(TEXT
						("Level : %d"), CurrentLevelIndex+1)));
				}

				// 웨이브 위젯 텍스트 설정
				UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave")));
				if (WaveText)
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT
						("Wave : %d"), WaveCount)));
				}
			}
		}
	}
}

void APracGameState::OnGameOver()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		auto* PracPlayerController = Cast<APracPlayerController>(PlayerController);
		if (PracPlayerController)
		{
			PracPlayerController->SetPause(true);
			PracPlayerController->ShowMainMenu(true);
		}
	}
}
