#pragma once

#include "Character/FPSCharacter.h"
#include "AIFPSCharacter.generated.h"

UCLASS()
class BTFPS_API AAIFPSCharacter : public AFPSCharacter
{
	GENERATED_BODY()
public:

	virtual void OnDie() override;

	class AAIController* GetAIController() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Damage Taken"))
	void BP_OnDamageTaken();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Dead"))
	void BP_OnDead();

	UFUNCTION(BlueprintCallable)
	class UAnimMontage* GetRandomHitAnimation() const;

	UFUNCTION(BlueprintCallable)
	class UAnimMontage* GetRandomDeathAnimation() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TArray<class UAnimMontage*> HitReactions;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TArray<class UAnimMontage*> DeathAnims;

	UFUNCTION()
	void DoRagdoll();

	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};

