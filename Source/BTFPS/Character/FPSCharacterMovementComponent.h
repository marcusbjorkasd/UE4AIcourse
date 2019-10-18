#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacterMovementComponent.generated.h"

UCLASS()
class BTFPS_API UFPSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	virtual float GetMaxSpeed() const override;

	float MovementSpeedModifier = 1.0f;
};

