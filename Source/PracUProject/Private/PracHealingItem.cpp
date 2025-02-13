// Fill out your copyright notice in the Description page of Project Settings.


#include "PracHealingItem.h"
#include "PracRPGCharacter.h"
#include "Blueprint/UserWidget.h"

// Sets default values
APracHealingItem::APracHealingItem()
{
	HealAmount = 20.0f;
	ItemType = "Healing";
}

void APracHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{
		APracRPGCharacter* PlayerCharacter = Cast<APracRPGCharacter>(Activator);
		if (PlayerCharacter)
		{
			// 체력 회복 애니메이션 재생
			if (PlayerCharacter->GetHealth() < PlayerCharacter->GetMaxHealth())
			{
				UUserWidget* OverHeadWidgetInstance = PlayerCharacter->GetOverHeadWidget();
				if (OverHeadWidgetInstance)
				{
					UFunction* PlayAnimFunc = OverHeadWidgetInstance->FindFunction(FName("PlayHealingHPAnim"));
					if (PlayAnimFunc)
					{
						OverHeadWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
					}
				}
			}

			// 체력 회복
			PlayerCharacter->AddHealth(HealAmount);
		}
		
		DestroyItem();
	}
}


