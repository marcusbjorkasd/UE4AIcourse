#pragma once

#include "GameFramework/Actor.h"
#include "FPSWeapon.generated.h"

UCLASS()
class BTFPS_API AFPSWeapon : public AActor
{
	GENERATED_BODY()
public:
	AFPSWeapon();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh = nullptr;

	UPROPERTY(EditAnywhere, Category = Stats)
	float Range = 100000.0f;

	UPROPERTY(EditAnywhere, Category = Stats)
	float Damage = 20.0f;

	UPROPERTY(EditAnywhere, Category = Stats)
	float Interval = 0.2f;

	UPROPERTY(EditAnywhere, Category = Stats)
	int Ammo = 50.0;

	int CurrentAmmo = 50;

	UPROPERTY(EditAnywhere, Category = VFX)
	UParticleSystem* MuzzleFX = nullptr;

	UPROPERTY(EditAnywhere, Category = VFX)
	FName MuzzleFXSocketName = FName(TEXT("MuzzleFlashSocket"));

	UPROPERTY(EditAnywhere, Category = VFX)
	UParticleSystem* TrailEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = VFX)
	FName TrailParamName = FName(TEXT("ShockBeamEnd"));

	float CurrentInterval = 0.0f;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnFiredBullet(const FVector& HitLocation, AActor* HitActor);

	UFUNCTION(BlueprintCallable)
	bool Reload();

	UFUNCTION(BlueprintCallable)
	void Attack(class APawn* AttackInstigator, const FVector& StartLocation, const FVector& ForwardVector);

	UFUNCTION(BlueprintPure)
	bool CanShoot() const;

	UFUNCTION(BlueprintPure)
	int GetCurrentAmmo() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil, meta = (ClampMin = -1.0, ClampMax = 0.0f))
	float RangeHorizontalMinimum = -0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil, meta = (ClampMin = 0.0, ClampMax = 1.0f))
	float RangeHorizontalMaximum = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil, meta = (ClampMin = -1.0, ClampMax = 0.0f))
	float RangeVerticalMinimum = -0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil, meta = (ClampMin = 0.0, ClampMax = 1.0f))
	float RangeVerticalMaximum = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil)
	float KickDistance = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil)
	float SpeedDeflect = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Recoil)
	float SpeedRecover = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Clamp")
	float YawMin = -200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Clamp")
	float YawMax = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Clamp")
	float PitchMin = -200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Clamp")
	float PitchMax = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsRifle = true;
};
