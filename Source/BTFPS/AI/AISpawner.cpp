#include "AISpawner.h"
#include "AI/AIFPSCharacter.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

void AAISpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay)
	{
		SpawnCharacter();
	}
}

void AAISpawner::Destroyed()
{
	if (UWorld * World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	Super::Destroyed();
}

void AAISpawner::SpawnCharacterOverride(int32 InCount, float InDuration, bool bInRandomDuration, float InRandomDurationMin, float InRandomDurationMax)
{
	SpawnCharacterImpl(InCount, InDuration, bInRandomDuration, InRandomDurationMin, InRandomDurationMax);
}

void AAISpawner::SpawnCharacter()
{
	SpawnCharacterImpl(Count, Duration, bRandomDuration, RandomDurationMin, RandomDurationMax);
}

void AAISpawner::SpawnCharacterImpl(int32 InCount, float InDuration, bool bInRandomDuration, float InMinDuration, float InMaxDuration)
{
	for (int32 Index = 0; Index < InCount; ++Index)
	{
		StartSpawnCharacterTimer(bInRandomDuration ? FMath::RandRange(InMinDuration, InMaxDuration) : InDuration);
	}
}

void AAISpawner::StartSpawnCharacterTimer(float InDuration)
{
	if (InDuration > 0.0f)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAISpawner::HandleSpawnTimerDone, InDuration, false);
	}
	else
	{
		HandleSpawnTimerDone();
	}
}

void AAISpawner::HandleSpawnTimerDone()
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (AAIFPSCharacter * SpawnedCharacter = GetWorld()->SpawnActor<AAIFPSCharacter>(CharacterClass, GetActorTransform(), Params))
	{
		BP_OnSpawned(SpawnedCharacter);
	}
}
