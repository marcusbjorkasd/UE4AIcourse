#pragma once

#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AITypes.h"
#include "FPSAIController.generated.h"

USTRUCT(BlueprintType)
struct FAITargetInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = Info)
	AActor* TargetActor = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = Info)
	float Aggro = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = Info)
	bool bVisible = false;
};

UCLASS()
class BTFPS_API AAIFPSController : public AAIController
{
	GENERATED_BODY()
public:
	AAIFPSController(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void GetActorEyesViewPoint(FVector& out_Location, FRotator& out_Rotation) const override;

	UFUNCTION()
	void OnActorPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	class AAIFPSCharacter* GetAICharacter() const;

	UFUNCTION(BlueprintPure)
	bool GetTargetWithHighestAggro(FAITargetInfo& OutTargetInfo) const;

	UFUNCTION(BlueprintPure)
	const TArray<FAITargetInfo>& GetAllTargets() const { return Targets; };

	UFUNCTION(BlueprintPure)
	TArray<FAITargetInfo> GetAllVisibleTargets() const;

	bool HasTarget(AActor* TargetActor) const;
	void AddNewTarget(AActor* NewTarget, bool bVisible);
	void AddAggro(AActor* TargetActor, float Aggro);
	void UpdateTarget(AActor* TargetActor, bool bVisible);

protected:
	TArray<FAITargetInfo> Targets;
};
