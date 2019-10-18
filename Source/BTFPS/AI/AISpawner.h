#pragma once

#include "GameFramework/Actor.h"
#include "AISpawner.generated.h"

UCLASS()
class BTFPS_API AAISpawner : public AActor
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	// Spawn and override default settings.
	UFUNCTION(BlueprintCallable, Category = Spawner)
	void SpawnCharacterOverride(int32 Count = 1, float Duration = 0.0f, bool bRandomDuration = false, float RandomDurationMin = 0.0f, float RandomDurationMax = 1.0f);
	
	// Spawn with default settings.
	UFUNCTION(BlueprintCallable, Category = Spawner)
	void SpawnCharacter();

	// Called every time a character is spawned.
	UFUNCTION(BlueprintImplementableEvent, Category = Spawner, meta = (DisplayName = "On Spawned"))
	void BP_OnSpawned(class AAIFPSCharacter* AICharacter);

private:
	void SpawnCharacterImpl(int32 Count, float Duration, bool bRandomDuration, float RandomDurationMin, float RandomDurationMax);
	void StartSpawnCharacterTimer(float Duration);

	UFUNCTION()
	void HandleSpawnTimerDone();

	UPROPERTY(EditAnywhere, Category = Spawn)
	TSubclassOf<class AAIFPSCharacter> CharacterClass;

	UPROPERTY(EditAnywhere, Category = Spawn)
	int32 Count = 1;

	UPROPERTY(EditAnywhere, Category = Spawn)
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, Category = Spawn)
	float RandomDurationMin = 0.0f;

	UPROPERTY(EditAnywhere, Category = Spawn)
	float RandomDurationMax = 0.0f;

	UPROPERTY(EditAnywhere, Category = Spawn)
	bool bSpawnOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = Spawn)
	bool bRandomDuration = false;
};
