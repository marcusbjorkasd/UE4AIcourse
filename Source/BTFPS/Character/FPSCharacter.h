#pragma once

#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "FPSCharacter.generated.h"

UCLASS()
class BTFPS_API AFPSCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AFPSCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(static_cast<uint8>(Team)); };

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class AFPSWeapon> StartingWeapon;

	UFUNCTION(BlueprintCallable)
	void EquipNewWeapon(TSubclassOf<class AFPSWeapon> NewWeapon);

	UFUNCTION(BlueprintCallable)
	bool TryToAttackWithWeapon();

	UFUNCTION(BlueprintPure)
	class AFPSWeapon* GetWeapon() const;

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	void Heal(float AmountToHeal);

	virtual void OnDie();

	UFUNCTION(BlueprintPure)
	int32 GetTeam() const { return Team; }

	UFUNCTION(BlueprintPure)
	bool WantsToAttack() const { return bWantsToAttack; }

	UFUNCTION(BlueprintPure)
	bool IsRunning() const { return bIsRunning; }

	UFUNCTION(BlueprintPure)
	bool GetADS() const { return bADS; }

	UFUNCTION(BlueprintPure)
	float GetMovementSpeedModifier() const;

	class UFPSCharacterMovementComponent* GetFPSCharacterMovementComponent() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Fire With Weapon"))
	void BP_OnFireWithWeapon();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Reload Weapon"))
	void BP_OnReloadWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Team)
	int32 Team = 0;

protected:

	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void LookRight(float Value);
	void AttackPressed();
	void AttackReleased();
	void ADSPressed();
	void ADSReleased();
	void SprintPressed();
	void SprintReleased();

	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	class UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	class USkeletalMeshComponent* Arms = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Recoil)
	class URecoilComponent* RecoilComponent = nullptr;

	UFUNCTION(BlueprintPure)
	class AFPSWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UPROPERTY(Transient)
	class AFPSWeapon* CurrentWeapon = nullptr;

	UFUNCTION(BlueprintCallable)
	void Reload();

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	FName WeaponArmsAttachName = FName(TEXT("WeaponPoint"));
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	FName WeaponBodyAttachName = FName(TEXT("WeaponPoint"));

	UPROPERTY(EditAnywhere, Category = Health)
	float Health = 100.0f;
	float CurrentHealth = 1.0f;

	bool bDead = false;

	bool bWantsToAttack = false;

	bool bIsRunning = false;

	bool bADS = false;
};
