#include "FPSAIController.h"
#include "AI/AIFPSCharacter.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"

AAIFPSController::AAIFPSController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PerceptionComponent = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("PerceptionComponent"));
	PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AAIFPSController::OnActorPerceptionUpdated);
}

void AAIFPSController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int32 Index = Targets.Num() - 1; Index >= 0; --Index)
	{
		FAITargetInfo& TargetInfo = Targets[Index];

		if (TargetInfo.Aggro < 0.0f && !TargetInfo.bVisible)
		{
			Targets.RemoveAt(Index);
		}
		else if (!TargetInfo.bVisible)
		{
			TargetInfo.Aggro -= DeltaTime;
		}
	}
}

void AAIFPSController::GetActorEyesViewPoint(FVector& out_Location, FRotator& out_Rotation) const
{
	AAIFPSCharacter* AICharacter = GetAICharacter();

	if (AICharacter == nullptr)
	{
		Super::GetActorEyesViewPoint(out_Location, out_Rotation);
	}
	else
	{
		out_Location = AICharacter->GetCapsuleComponent()->GetComponentLocation();
		out_Rotation = AICharacter->GetActorRotation();
	}
}

void AAIFPSController::OnActorPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!HasTarget(Actor))
	{
		AddNewTarget(Actor, Stimulus.WasSuccessfullySensed());
	}
	else
	{
		UpdateTarget(Actor, Stimulus.WasSuccessfullySensed());
	}
}

ETeamAttitude::Type AAIFPSController::GetTeamAttitudeTowards(const AActor& Other) const
{
	ETeamAttitude::Type Attitude = ETeamAttitude::Hostile;

	const AAIFPSCharacter* MyCharacter = GetAICharacter();
	const AFPSCharacter* OtherCharacter = Cast<const AFPSCharacter>(&Other);

	if (MyCharacter && OtherCharacter)
	{
		if (MyCharacter->Team == OtherCharacter->Team)
		{
			Attitude = ETeamAttitude::Friendly;
		}
	}

	return Attitude;
}

AAIFPSCharacter* AAIFPSController::GetAICharacter() const
{
	return CastChecked<AAIFPSCharacter>(GetPawn(), ECastCheckedType::NullAllowed);
}

bool  AAIFPSController::GetTargetWithHighestAggro(FAITargetInfo& OutTargetInfo) const
{
	OutTargetInfo = FAITargetInfo();
	
	float HighestAggro = 0.0f;
	bool bFoundTarget = false;

	for (const FAITargetInfo& TargetInfo : Targets)
	{
		if (TargetInfo.Aggro > HighestAggro)
		{
			HighestAggro = TargetInfo.Aggro;
			OutTargetInfo = TargetInfo;
		}
	}

	return bFoundTarget;
}

TArray<FAITargetInfo> AAIFPSController::GetAllVisibleTargets() const
{
	TArray<FAITargetInfo> Out;

	for (const FAITargetInfo& TargetInfo : Targets)
	{
		if (TargetInfo.bVisible)
		{
			Out.Add(TargetInfo);
		}
	}

	return Out;
}

bool AAIFPSController::HasTarget(AActor* TargetActor) const
{
	for (const FAITargetInfo& TargetInfo : Targets)
	{
		if (TargetInfo.TargetActor == TargetActor)
		{
			return true;
		}
	}

	return false;
}

void AAIFPSController::AddNewTarget(AActor* NewTarget, bool bVisible)
{
	FAITargetInfo TargetInfo;
	TargetInfo.TargetActor = NewTarget;
	TargetInfo.Aggro = 50.0f;
	TargetInfo.bVisible = bVisible;
	Targets.Add(TargetInfo);
}

void AAIFPSController::AddAggro(AActor* TargetActor, float Aggro)
{
	if (!HasTarget(TargetActor))
	{
		return;
	}
	
	for (FAITargetInfo& TargetInfo : Targets)
	{
		if (TargetInfo.TargetActor == TargetActor)
		{
			TargetInfo.Aggro += Aggro;
		}
	}
}

void AAIFPSController::UpdateTarget(AActor* TargetActor, bool bVisible)
{
	for (FAITargetInfo& TargetInfo : Targets)
	{
		if (TargetInfo.TargetActor == TargetActor)
		{
			TargetInfo.bVisible = bVisible;
		}
	}
}

