#include "RecoilComponent.h"
#include "Weapon/FPSWeapon.h"
#include "GameFramework/Pawn.h"

static FVector2D VRand2D(float MinX, float MaxX, float MinY, float MaxY, UWorld* World = nullptr)
{
	FVector2D Result;

	float L;

	do
	{
		Result.X = FMath::RandRange(MinX, MaxX);
		Result.Y = FMath::RandRange(MinY, MaxY);

		L = Result.SizeSquared();
	} while (L > 1.0f || L < KINDA_SMALL_NUMBER);

	return Result * (1.0f / FGenericPlatformMath::Sqrt(L));
}

URecoilComponent::URecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void URecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
}

void URecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerPawn != nullptr && Weapon != nullptr)
	{
		// Add view kick
		const FVector2D Movement = ViewKickUpdate(DeltaTime);
		OwnerPawn->AddControllerYawInput(Movement.X);
		OwnerPawn->AddControllerPitchInput(-Movement.Y);
	}
}

void URecoilComponent::AddRecoil(class AFPSWeapon* InWeapon)
{
	ViewKickApply(InWeapon);
}

void URecoilComponent::HandleTurn(float Value)
{
	if (!bIsFiring)
	{
		return;
	}

	if (Value > 0.0f)
	{
		AccumulatedInputHorizontalMax += Value;
	}
	else
	{
		AccumulatedInputHorizontalMin += Value;
	}
}

void URecoilComponent::HandleLookUp(float Value)
{
	if (!bIsFiring)
	{
		return;
	}
	if (Value > 0.0f)
	{
		AccumulatedInputVerticalMax += Value;
	}
	else
	{
		AccumulatedInputVerticalMin += Value;
	}
}

void URecoilComponent::HandleFireStart()
{
	bIsFiring = true;
}

void URecoilComponent::HandleFireStop()
{
	bIsFiring = false;
}

void URecoilComponent::ViewKickApply(AFPSWeapon* InWeapon)
{
	Weapon = InWeapon;
	ViewKickSpeed = Weapon->SpeedDeflect;
	ViewKickVelocity = FVector2D::ZeroVector;

	const FVector2D RandomDirection = VRand2D(Weapon->RangeHorizontalMinimum, Weapon->RangeHorizontalMaximum, Weapon->RangeVerticalMinimum, Weapon->RangeVerticalMaximum, GetWorld()).GetSafeNormal();

	ViewKickTarget = ViewKick + (RandomDirection * (Weapon->KickDistance));

	ViewKickTarget.X = FMath::Clamp(ViewKickTarget.X, Weapon->YawMin - AccumulatedInputHorizontalMax, Weapon->YawMax - AccumulatedInputHorizontalMin);
	ViewKickTarget.Y = FMath::Clamp(ViewKickTarget.Y, Weapon->PitchMin + AccumulatedInputVerticalMin, Weapon->PitchMax + AccumulatedInputVerticalMax);
}

const FVector2D URecoilComponent::ViewKickUpdate(float DeltaTime)
{
	const FVector2D Distance = ViewKickTarget - ViewKick;

	ViewKickVelocity += Distance * DeltaTime;
	float CurveValue = 1.0f;
	FVector2D Movement = ViewKickVelocity * ViewKickSpeed * DeltaTime;

	ViewKick += Movement;

	if (!bIsFiring && (FVector2D::DotProduct(ViewKickTarget - ViewKick, Distance)) <= 0.1f)
	{
		ViewKick = ViewKickTarget;
		if (!bIsFiring)
		{
			if (ViewKick.Y > 0.0f && AccumulatedInputVerticalMax > 0.0f)
			{
				ViewKick.Y = FMath::Max(ViewKick.Y - AccumulatedInputVerticalMax, 0.0f);
			}
			else if (ViewKick.Y < 0.0f && AccumulatedInputVerticalMin < 0.0f)
			{
				ViewKick.Y = FMath::Max(ViewKick.Y - AccumulatedInputVerticalMin, 0.0f);
			}

			if (ViewKick.X > 0.0f && AccumulatedInputHorizontalMin < 0.0f)
			{
				ViewKick.X = FMath::Max(ViewKick.X + AccumulatedInputHorizontalMin, 0.0f);
			}
			else if (ViewKick.X < 0.0f && AccumulatedInputHorizontalMax > 0.0f)
			{
				ViewKick.X = FMath::Min(ViewKick.X + AccumulatedInputHorizontalMax, 0.0f);
			}

			ZeroAccumulatedInput();
		}

		ViewKickVelocity = FVector2D::ZeroVector;
		ViewKickTarget = FVector2D::ZeroVector;
		ViewKickSpeed = Weapon->SpeedRecover;
		return Distance * CurveValue;
	}

	return Movement;
}
