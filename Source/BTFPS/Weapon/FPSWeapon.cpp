#include "FPSWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"

AFPSWeapon::AFPSWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AFPSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentInterval = FMath::Max(CurrentInterval - DeltaTime, 0.0f);
}

void AFPSWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = Ammo;
}

bool AFPSWeapon::Reload()
{
	if (CurrentAmmo != Ammo)
	{
		CurrentAmmo = Ammo;
		return true;
	}

	return false;
}

void AFPSWeapon::Attack(APawn* AttackInstigator, const FVector& StartLocation, const FVector& ForwardVector)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(AttackInstigator);

	FHitResult Hit;
	const FVector EndLocation = StartLocation + (ForwardVector * Range);

	FCollisionQueryParams Params;
	Params.AddIgnoredActors(IgnoreActors);

	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, Params);

	const FVector ImpactLocation = Hit.bBlockingHit ? Hit.ImpactPoint : EndLocation;

	if (TrailEffect != nullptr)
	{
		const FVector TrailEffectStart = Mesh->GetSocketLocation(MuzzleFXSocketName);
		UParticleSystemComponent* Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailEffect, TrailEffectStart);
		if (Particle != nullptr)
		{
			Particle->SetVectorParameter(TrailParamName, ImpactLocation);
		}
	}

	if (MuzzleFX != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh, MuzzleFXSocketName);
	}

	if (Hit.bBlockingHit && Hit.Actor.IsValid())
	{
		UGameplayStatics::ApplyPointDamage(Hit.Actor.Get(), Damage, GetActorLocation(), Hit, AttackInstigator->Controller, this, UDamageType::StaticClass());
	}

	CurrentInterval = Interval;
	CurrentAmmo -= 1;

	BP_OnFiredBullet(ImpactLocation, Hit.Actor.Get());
}

bool AFPSWeapon::CanShoot() const
{
	return CurrentInterval <= 0.0f && CurrentAmmo > 0;
}

int AFPSWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

