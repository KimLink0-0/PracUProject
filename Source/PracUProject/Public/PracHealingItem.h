// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PracBaseItem.h"
#include "PracHealingItem.generated.h"

UCLASS()
class PRACUPROJECT_API APracHealingItem : public APracBaseItem
{
	GENERATED_BODY()
	
public:	
	APracHealingItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Healing")
	int32 HealAmount;

	virtual void ActivateItem(AActor* Activator) override;
};
