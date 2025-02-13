// Fill out your copyright notice in the Description page of Project Settings.


#include "PracBigCoinItem.h"

// Sets default values
APracBigCoinItem::APracBigCoinItem()
{
	PointValue = 50;
	ItemType = "BigCoin";
}

void APracBigCoinItem::ActivateItem(class AActor* Activator)
{
	Super::ActivateItem(Activator);
}





