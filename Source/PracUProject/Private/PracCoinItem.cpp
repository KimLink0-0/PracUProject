// Fill out your copyright notice in the Description page of Project Settings.


#include "PracCoinItem.h"
#include "PracGameState.h"

// Sets default values
APracCoinItem::APracCoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void APracCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			APracGameState* GameState = World->GetGameState<APracGameState>();
			if (GameState)
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}

		DestroyItem();
	}
}