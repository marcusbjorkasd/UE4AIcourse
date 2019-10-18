#pragma once

#include "Components/ActorComponent.h"
#include "RecoilComponent.generated.h"

UCLASS()
class BTFPS_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	URecoilComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddRecoil(class AFPSWeapon* Weapon);

	void HandleTurn(float Value);
	void HandleLookUp(float Value);

	void HandleFireStart();
	void HandleFireStop();

private:

	void ViewKickApply(class AFPSWeapon* Weapon);
	const FVector2D ViewKickUpdate(float DeltaTime);

	FVector2D GunKick;
	FVector2D ViewKick;

	FVector2D ViewKickTarget;
	FVector2D ViewKickVelocity;

	UPROPERTY(Transient)
	class APawn* OwnerPawn = nullptr;

	UPROPERTY(Transient)
	class AFPSWeapon* Weapon = nullptr;

	FORCEINLINE void ZeroAccumulatedInput()
	{
		AccumulatedInputVerticalMax = 0.0f;
		AccumulatedInputVerticalMin = 0.0f;
		AccumulatedInputHorizontalMax = 0.0f;
		AccumulatedInputHorizontalMin = 0.0f;
	}

	float ViewKickSpeed = 0.0f;
	float KickBackVelocity = 0.0f;
	float KickBackRecovery = 0.0f;

	float AccumulatedInputVerticalMax = 0.0f;
	float AccumulatedInputVerticalMin = 0.0f;
	float AccumulatedInputHorizontalMax = 0.0f;
	float AccumulatedInputHorizontalMin = 0.0f;

	bool bIsFiring = false;
};
