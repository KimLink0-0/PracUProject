// Fill out your copyright notice in the Description page of Project Settings.


#include "PracSmallCoinItem.h"

// Sets default values
APracSmallCoinItem::APracSmallCoinItem()
{
	PointValue = 10;
	ItemType = "SmallCoin";
}

void APracSmallCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}



