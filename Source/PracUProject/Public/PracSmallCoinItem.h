// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PracCoinItem.h"
#include "PracSmallCoinItem.generated.h"

UCLASS()
class PRACUPROJECT_API APracSmallCoinItem : public APracCoinItem
{
	GENERATED_BODY()
	
public:	
	APracSmallCoinItem();

protected:

	virtual void ActivateItem(AActor* Activator) override;
};
