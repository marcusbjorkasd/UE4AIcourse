#include "AIFPSCharacter.h"
#include "Controller/FPSAIController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Weapon/FPSWeapon.h"

void AAIFPSCharacter::OnDie()
{
	Super::OnDie();

	bDead = true;
	BP_OnDead();

	if (AAIController * AIController = GetAIController())
	{
		UBrainComponent* Brain = AIController->BrainComponent;
		if (Brain != nullptr)
		{
			Brain->StopLogic(FString(TEXT("Death")));
		}
	}

	PlayAnimMontage(GetRandomDeathAnimation());

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAIFPSCharacter::DoRagdoll, FMath::RandRange(0.25f, 0.5f), false);
}

AAIController* AAIFPSCharacter::GetAIController() const
{
	return CastChecked<AAIController>(Controller, ECastCheckedType::NullAllowed);
}

class UAnimMontage* AAIFPSCharacter::GetRandomHitAnimation() const
{
	return HitReactions[FMath::RandRange(0, HitReactions.Num() - 1)];
}

class UAnimMontage* AAIFPSCharacter::GetRandomDeathAnimation() const
{
	return DeathAnims[FMath::RandRange(0, DeathAnims.Num() - 1)];
}

void AAIFPSCharacter::DoRagdoll()
{
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("NoCollision")));
	GetMesh()->SetCollisionProfileName(FName(TEXT("PhysicsActor")));
	GetMesh()->SetSimulatePhysics(true);
	SetLifeSpan(10.0f);

	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->SetLifeSpan(10.0f);
		CurrentWeapon->Mesh->SetCollisionProfileName(FName(TEXT("PhysicsActor")));
		CurrentWeapon->Mesh->SetSimulatePhysics(true);
		CurrentWeapon = nullptr;
	}
}

float AAIFPSCharacter::InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float OutDamage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);

	if (!IsDead())
	{
		PlayAnimMontage(GetRandomHitAnimation());

		if (Health <= 0.0f)
		{
			OnDie();
		}
		else
		{
			if (AAIFPSController * AIController = Cast<AAIFPSController>(GetAIController()))
			{
				AIController->AddAggro(EventInstigator->GetPawn(), 20.0f);
			}

			BP_OnDamageTaken();
		}
	}

	return OutDamage;
}

