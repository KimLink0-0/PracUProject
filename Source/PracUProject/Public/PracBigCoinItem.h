// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PracCoinItem.h"
#include "PracBigCoinItem.generated.h"

UCLASS()
class PRACUPROJECT_API APracBigCoinItem : public APracCoinItem
{
	GENERATED_BODY()
	
public:	
	APracBigCoinItem();

protected:

	virtual void ActivateItem(class AActor* Activator) override;
};
