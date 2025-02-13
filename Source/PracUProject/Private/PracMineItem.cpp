// Fill out your copyright notice in the Description page of Project Settings.


#include "PracMineItem.h"
#include "Blueprint/UserWidget.h"
#include "PracRPGCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
APracMineItem::APracMineItem()
{
	ItemType = "Mine";
	
	ExplosionCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollisionComp"));
	ExplosionCollisionComp->InitSphereRadius(ExplosionRadius);
	ExplosionCollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollisionComp->SetupAttachment(SceneComp);
}

void APracMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded)
	{
		return;
	}
	
	Super::ActivateItem(Activator);
	
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&APracMineItem::Explode,
		ExplosionDelay);

	bHasExploded = true;
}

void APracMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false);
	}
	
	TArray<AActor*> OverlappingActors;
	ExplosionCollisionComp->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass());

			APracRPGCharacter* PlayerCharacter = Cast<APracRPGCharacter>(Actor);
			UUserWidget* OverHeadWidgetInstance = PlayerCharacter->GetOverHeadWidget();
			UFunction* PlayAnimFunc = OverHeadWidgetInstance->FindFunction(FName("PlayDamagedHPAnim"));
			if (PlayAnimFunc)
			{
				OverHeadWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}
		}
	}

	DestroyItem();
	
	if (Particle)
	{
		FTimerHandle DestroyTimerHandle;
		
		TWeakObjectPtr<UParticleSystemComponent> WeakParticlePtr = Particle;
		GetWorld()->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			[WeakParticlePtr]()
			{
				if (WeakParticlePtr.IsValid())
				{
					WeakParticlePtr->DestroyComponent();
				}
			},
			2.0f,
			false);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation());
	}
}


