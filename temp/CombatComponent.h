// CombatComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UAnimMontage;
class UStaminaComponent;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Idle,
	Attacking,
	Recovery
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ROMANERAGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LightAttackStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float HeavyAttackStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* LightAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	ECombatState State;

	TWeakObjectPtr<ACharacter> OwningCharacter;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnAttackStarted OnAttackStarted;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnAttackEnded OnAttackEnded;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TryLightAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TryHeavyAttack();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	ECombatState GetState() const { return State; }
};
