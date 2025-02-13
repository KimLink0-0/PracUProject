// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PracBaseItem.h"
#include "PracMineItem.generated.h"

UCLASS()
class PRACUPROJECT_API APracMineItem : public APracBaseItem
{
	GENERATED_BODY()
	
public:	
	APracMineItem();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* ExplosionCollisionComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Mine")
	float ExplosionDelay = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Mine")
	float ExplosionRadius = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Mine")
	float ExplosionDamage = 50.0f;

	bool bHasExploded = false;
	FTimerHandle ExplosionTimerHandle;

	virtual void ActivateItem(AActor* Activator) override;

	void Explode();
};
