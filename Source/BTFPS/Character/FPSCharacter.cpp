#include "FPSCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "FPSCharacterMovementComponent.h"
#include "Weapon/FPSWeapon.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "Components/RecoilComponent.h"

AFPSCharacter::AFPSCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFPSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());

	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(CameraComponent);

	RecoilComponent = CreateDefaultSubobject<URecoilComponent>(TEXT("RecoilComponent"));

	bUseControllerRotationPitch = true;
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = Health;

	EquipNewWeapon(StartingWeapon);
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWantsToAttack)
	{
		TryToAttackWithWeapon();
	}
}

void AFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AFPSCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AFPSCharacter::LookRight);

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AFPSCharacter::AttackPressed);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &AFPSCharacter::AttackReleased);

	PlayerInputComponent->BindAction(TEXT("ADS"), IE_Pressed, this, &AFPSCharacter::ADSPressed);
	PlayerInputComponent->BindAction(TEXT("ADS"), IE_Released, this, &AFPSCharacter::ADSReleased);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AFPSCharacter::SprintPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AFPSCharacter::SprintReleased);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AFPSCharacter::Reload);
}

void AFPSCharacter::EquipNewWeapon(TSubclassOf<class AFPSWeapon> NewWeaponClass)
{
	if (NewWeaponClass.Get() == nullptr)
	{
		return;
	}

	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	AFPSWeapon* NewWeaponInstance = GetWorld()->SpawnActor<AFPSWeapon>(NewWeaponClass);

	if (NewWeaponInstance != nullptr)
	{
		CurrentWeapon = NewWeaponInstance;
		if (IsPlayerControlled())
		{
			CurrentWeapon->AttachToComponent(Arms, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponArmsAttachName);
		}
		else
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponBodyAttachName);
		}
	}
}

bool AFPSCharacter::TryToAttackWithWeapon()
{
	if (CurrentWeapon == nullptr)
	{
		return false;
	}

	if (CurrentWeapon->CanShoot())
	{
		FVector ViewLocation;
		FRotator ViewRotation;
		Controller->GetActorEyesViewPoint(ViewLocation, ViewRotation);
		CurrentWeapon->Attack(this, ViewLocation, ViewRotation.Vector());
		BP_OnFireWithWeapon();

		if (IsPlayerControlled())
		{
			RecoilComponent->AddRecoil(CurrentWeapon);
		}
	}

	return false;
}

AFPSWeapon* AFPSCharacter::GetWeapon() const
{
	return CurrentWeapon;
}

bool AFPSCharacter::IsDead() const
{
	return bDead;
}

float AFPSCharacter::GetHealth() const
{
	return CurrentHealth;
}

void AFPSCharacter::Heal(float AmountToHeal)
{
	CurrentHealth = FMath::Min(CurrentHealth + AmountToHeal, Health);
}

void AFPSCharacter::OnDie()
{

}

float AFPSCharacter::GetMovementSpeedModifier() const
{
	return GetFPSCharacterMovementComponent()->MovementSpeedModifier;
}

UFPSCharacterMovementComponent* AFPSCharacter::GetFPSCharacterMovementComponent() const
{
	return CastChecked<UFPSCharacterMovementComponent>(GetMovementComponent());
}

float AFPSCharacter::InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Health -= Damage;
	return Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);
}

void AFPSCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AFPSCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void AFPSCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
	RecoilComponent->HandleLookUp(Value);
}

void AFPSCharacter::LookRight(float Value)
{
	AddControllerYawInput(Value);
	RecoilComponent->HandleTurn(Value);
}

void AFPSCharacter::AttackPressed()
{
	bWantsToAttack = true;
	RecoilComponent->HandleFireStart();
}

void AFPSCharacter::AttackReleased()
{
	bWantsToAttack = false;
	RecoilComponent->HandleFireStop();
}

void AFPSCharacter::ADSPressed()
{
	bADS = true;
}

void AFPSCharacter::ADSReleased()
{
	bADS = false;
}

void AFPSCharacter::SprintPressed()
{
	GetFPSCharacterMovementComponent()->MovementSpeedModifier = 1.2f;
	bIsRunning = true;
}

void AFPSCharacter::SprintReleased()
{
	GetFPSCharacterMovementComponent()->MovementSpeedModifier = 1.0f;
	bIsRunning = false;
}

void AFPSCharacter::Reload()
{
	if (CurrentWeapon != nullptr)
	{
		if (CurrentWeapon->Reload())
		{
			BP_OnReloadWeapon();
		}
	}
}
