// Fill out your copyright notice in the Description page of Project Settings.


#include "PracPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "PracGameInstance.h"
#include "PracGameState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

APracPlayerController::APracPlayerController()
	: InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, LookAction(nullptr)
	, JumpAction(nullptr)
	, CrouchAction(nullptr)
	, SprintAction(nullptr)
	, HUDWidgetClass(nullptr)
	, HUDWidgetInstance(nullptr)
	, MainMenuWidgetClass(nullptr)
	, MainMenuWidgetInstance(nullptr)
{
}

void APracPlayerController::ShowGameHUD()
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
		}
	}

	APracGameState* PracGameState = GetWorld() ? GetWorld()->GetGameState<APracGameState>() : nullptr;
	if (PracGameState)
	{
		PracGameState->UpdateHUD();
	}
}

void APracPlayerController::ShowMainMenu(bool bIsRestart)
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

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}

			if (bIsRestart)
			{
				auto* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText"));
				if (TotalScoreText)
				{
					auto* PracGameInstance = Cast<UPracGameInstance>(UGameplayStatics::GetGameInstance(this));
					if (PracGameInstance)
					{
						TotalScoreText->SetText(FText::FromString(FString::Printf(
							TEXT("TotalScore : %d"), PracGameInstance->TotalScore)));
					}
				}

				UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
				if (PlayAnimFunc)
				{
					MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
				}
			}
		}
	}
}

void APracPlayerController::StartGame()
{
	auto* PracGameInstance = Cast<UPracGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (PracGameInstance)
	{
		PracGameInstance->CurrentLevelIndex = 0;
		PracGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
}

UUserWidget* APracPlayerController::GetMainMenuWidget() const
{
	return MainMenuWidgetInstance;
}

UUserWidget* APracPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void APracPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		auto* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (EnhancedInputSubsystem)
		{
			if (InputMappingContext)
			{
				EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}
