// Fill out your copyright notice in the Description page of Project Settings.


#include "PracGameInstance.h"

UPracGameInstance::UPracGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UPracGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Total Score Updated : %d"), TotalScore);
}
