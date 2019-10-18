#include "FPSCharacterMovementComponent.h"

float UFPSCharacterMovementComponent::GetMaxSpeed() const
{
	return Super::GetMaxSpeed() * MovementSpeedModifier;
}

