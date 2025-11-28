// StaminaComponent.cpp

#include "StaminaComponent.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxStamina = 100.f;
	CurrentStamina = MaxStamina;

	RegenRate = 25.f;
	RegenDelaySeconds = 0.75f;
	TimeSinceLastSpend = 0.f;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
}

void UStaminaComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentStamina >= MaxStamina)
	{
		return;
	}

	TimeSinceLastSpend += DeltaTime;

	if (TimeSinceLastSpend >= RegenDelaySeconds)
	{
		CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + RegenRate * DeltaTime);
	}
}

bool UStaminaComponent::CanSpend(float Amount) const
{
	return Amount <= 0.f || CurrentStamina >= Amount;
}

bool UStaminaComponent::Spend(float Amount)
{
	if (!CanSpend(Amount))
	{
		return false;
	}

	CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.f, MaxStamina);
	TimeSinceLastSpend = 0.f;

	return true;
}

void UStaminaComponent::RestoreFull()
{
	CurrentStamina = MaxStamina;
	TimeSinceLastSpend = 0.f;
}
