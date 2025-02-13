// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PracGameState.generated.h"


UCLASS()
class PRACUPROJECT_API APracGameState : public AGameState
{
	GENERATED_BODY()

public:
	APracGameState();
	
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 Score;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TArray<FName> LevelMapNames;
	
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	bool bIsFirststart = false;

	void StartLevel();
	void OnLevelTimeUp();
	void OnCoinCollected();
	void EndLevel();

	void UpdateHUD();

	//Wave 시스템
	void StartWave();
	void EndWave();
	void OnWaveTimeUP();

	int32 WaveCount = 0;
	int32 WaveDuration = 10.0f;

	FTimerHandle WaveTimerHandle;
	
};
