// CombatComponent.cpp

#include "CombatComponent.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "StaminaComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	LightAttackStaminaCost = 20.f;
	HeavyAttackStaminaCost = 35.f;

	State = ECombatState::Idle;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());
}

void UCombatComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// No per-frame logic yet; state machine is driven by animation.
}

bool UCombatComponent::TryLightAttack()
{
	if (State != ECombatState::Idle || !OwningCharacter.IsValid())
	{
		return false;
	}

	UStaminaComponent* Stamina = OwningCharacter->FindComponentByClass<UStaminaComponent>();
	if (!Stamina || !Stamina->Spend(LightAttackStaminaCost))
	{
		return false;
	}

	UAnimInstance* AnimInst =
		OwningCharacter->GetMesh() ? OwningCharacter->GetMesh()->GetAnimInstance() : nullptr;

	if (AnimInst && LightAttackMontage)
	{
		State = ECombatState::Attacking;

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UCombatComponent::OnAttackMontageEnded);

		AnimInst->Montage_Play(LightAttackMontage);
		AnimInst->Montage_SetEndDelegate(EndDelegate, LightAttackMontage);

		OnAttackStarted.Broadcast();
		return true;
	}

	return false;
}

bool UCombatComponent::TryHeavyAttack()
{
	if (State != ECombatState::Idle || !OwningCharacter.IsValid())
	{
		return false;
	}

	UStaminaComponent* Stamina = OwningCharacter->FindComponentByClass<UStaminaComponent>();
	if (!Stamina || !Stamina->Spend(HeavyAttackStaminaCost))
	{
		return false;
	}

	UAnimInstance* AnimInst =
		OwningCharacter->GetMesh() ? OwningCharacter->GetMesh()->GetAnimInstance() : nullptr;

	if (AnimInst && HeavyAttackMontage)
	{
		State = ECombatState::Attacking;

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UCombatComponent::OnAttackMontageEnded);

		AnimInst->Montage_Play(HeavyAttackMontage);
		AnimInst->Montage_SetEndDelegate(EndDelegate, HeavyAttackMontage);

		OnAttackStarted.Broadcast();
		return true;
	}

	return false;
}

void UCombatComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	State = ECombatState::Idle;
	OnAttackEnded.Broadcast();
}
