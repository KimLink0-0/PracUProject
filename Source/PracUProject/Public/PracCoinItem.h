// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PracBaseItem.h"
#include "PracCoinItem.generated.h"

UCLASS()
class PRACUPROJECT_API APracCoinItem : public APracBaseItem // APracBaseItem 에서 AActor 상속 중
{
	GENERATED_BODY()
	
public:	
	APracCoinItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Coin")
	int32 PointValue;

	virtual void ActivateItem(AActor* Activator) override;
};
