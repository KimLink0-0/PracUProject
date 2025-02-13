// Fill out your copyright notice in the Description page of Project Settings.


#include "PracBaseItem.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

APracBaseItem::APracBaseItem()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>("SceneComp");
	SetRootComponent(SceneComp);

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereComp->SetupAttachment(SceneComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	StaticMeshComp->SetupAttachment(SphereComp);

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &APracBaseItem::OnItemStartOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &APracBaseItem::OnItemEndOverlap);
}

// 구체적인 기능들은 자식 클래스에서 구현

void APracBaseItem::OnItemStartOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT
			("Overlap!!")));

		ActivateItem(OtherActor);
	}
}

void APracBaseItem::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void APracBaseItem::ActivateItem(AActor* Activator)
{
	UParticleSystemComponent* Particle = nullptr;
	
	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			true);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
			);
	}
	
	if (Particle)
	{
		// 지역변수로 선언하는 이유 : 멤버변수일 해당 함수가 실행될 때마다 TimerHandle 이 2.0f 으로 리셋되기 때문
		// 각각의 파티클이 개별적인 핸들을 가지고 있어야 함
		FTimerHandle DestroyTimerHandle;

		// WeakPtr 사용으로 객체가 소멸한 뒤 람다함수가 객체를 참조하는 상황을 방지
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
}

// 공통적인 기능들은 부모 클래스에서 구현

FName APracBaseItem::GetItemType() const
{
	return ItemType;
}

void APracBaseItem::DestroyItem()
{
	Destroy();
}



