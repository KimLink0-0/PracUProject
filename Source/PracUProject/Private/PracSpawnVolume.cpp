// Fill out your copyright notice in the Description page of Project Settings.


#include "PracSpawnVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
APracSpawnVolume::APracSpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>("SceneComp");
	SetRootComponent(SceneComp);

	SpawningBoxComp = CreateDefaultSubobject<UBoxComponent>("SpawningBoxComp");
	SpawningBoxComp->SetupAttachment(SceneComp);
}

AActor* APracSpawnVolume::SpawnRandomItem()
{
	if (const FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;
}

FVector APracSpawnVolume::GetRandomPointInVolume() const
{
	// Extent : 박스 너비의 절반 
	const FVector BoxExtent = SpawningBoxComp->GetScaledBoxExtent();
	const FVector BoxOrigin = SpawningBoxComp->GetComponentLocation();
	
	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z));
}

FItemSpawnRow* APracSpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable)
	{
		return nullptr;
	}
	// 1. 배열 가져오기
	TArray<FItemSpawnRow*> AllRows;
	// 우리가 불러올 데이터 정보가 어떤 종류의 데이터를 다루는지 알려주기 위해서 필요 (GetAllRows() 의 디버깅이나 로깅에 필요)
	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty())
	{
		return nullptr;
	}

	// 2. 전체 확률 합 구하기
	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	// 3. 0 ~ 전체 확률 사이의 값 구하기 
	const float RandValue = FMath::FRandRange(0.0f, TotalChance);

	// 4. 누적 확률을 계산해서 RandValue 값이 해당 범위에 위치하면 해당 Item 이 Spawn
	// SmallCoin = 0~30, BigCoin = 30~40, Mine = 40~80, Healing = 80~100
	float AccumulateChance = 0.0f;
	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue < AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr;
}

AActor* APracSpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator);

	return SpawnedActor;
}
