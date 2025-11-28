#include "BattalionActor.h"
#include "HeroCommanderCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ABattalionActor::ABattalionActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentSoldiers = 0;
	CurrentMorale   = 100.f;
	CurrentOrder    = EBattalionOrder::None;

	DesiredFollowRadius = 600.f;
	HardLeashRadius     = 1600.f;

	bMoraleUnbreakable        = false;
	bCausesFearAura           = false;
	bImmuneToFearAura         = false;
	FearRadius                = 800.f;
	FearMoraleDamagePerSecond = 8.f;

	AIDifficulty = EUnitAIDifficulty::Normal;
}

void ABattalionActor::BeginPlay()
{
	Super::BeginPlay();

	if (UnitType)
	{
		CurrentSoldiers              = UnitType->MaxSoldiers;
		CurrentMorale                = UnitType->BaseMorale;
		bMoraleUnbreakable           = UnitType->bMoraleUnbreakable;
		bCausesFearAura              = UnitType->bCausesFearAura;
		bImmuneToFearAura            = UnitType->bImmuneToFearAura;
		FearRadius                   = UnitType->FearRadius;
		FearMoraleDamagePerSecond    = UnitType->FearMoraleDamagePerSecond;
		AIDifficulty                 = UnitType->BaseAIDifficulty;
	}
}

void ABattalionActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsGeneralBodyguard && ProtectedHero)
	{
		TickBodyguardFollow(DeltaSeconds);
	}

	if (bCausesFearAura)
	{
		TickFearAura(DeltaSeconds);
	}
}

void ABattalionActor::Command_MoveTo(const FVector& WorldLocation)
{
	LastCommandLocation = WorldLocation;
	CurrentOrder        = EBattalionOrder::Move;

	// Temp: teleport battalion actor so you can see commands working.
	SetActorLocation(WorldLocation);
}

void ABattalionActor::Command_HoldPosition()
{
	CurrentOrder = EBattalionOrder::Hold;
}

void ABattalionActor::Command_Retreat()
{
	CurrentOrder = EBattalionOrder::Retreat;
}

void ABattalionActor::InitializeAsGeneralBodyguard(AHeroCommanderCharacter* InHero, UUnitTypeData* InUnitType)
{
	ProtectedHero       = InHero;
	UnitType            = InUnitType;
	bIsGeneralBodyguard = true;

	if (UnitType)
	{
		CurrentSoldiers              = UnitType->MaxSoldiers;
		CurrentMorale                = UnitType->BaseMorale;
		bMoraleUnbreakable           = UnitType->bMoraleUnbreakable;
		bCausesFearAura              = UnitType->bCausesFearAura;
		bImmuneToFearAura            = UnitType->bImmuneToFearAura;
		FearRadius                   = UnitType->FearRadius;
		FearMoraleDamagePerSecond    = UnitType->FearMoraleDamagePerSecond;
		AIDifficulty                 = UnitType->BaseAIDifficulty;
	}
}

void ABattalionActor::TickBodyguardFollow(float DeltaSeconds)
{
	if (!ProtectedHero)
	{
		return;
	}

	const FVector HeroLocation      = ProtectedHero->GetActorLocation();
	const FVector BattalionLocation = GetActorLocation();
	const float   Distance          = FVector::Dist(HeroLocation, BattalionLocation);

	// Stay close to the hero, but don't over-extend.
	if (Distance > DesiredFollowRadius)
	{
		FVector TargetLocation = HeroLocation;

		// Stay a bit behind the hero based on current vector.
		const FVector ToBattalion = (BattalionLocation - HeroLocation).GetSafeNormal();
		TargetLocation += ToBattalion * DesiredFollowRadius;

		if (Distance > HardLeashRadius)
		{
			// Hero sprinted far away – snap closer.
			SetActorLocation(TargetLocation);
		}
		else
		{
			// Smoothly move toward the target location.
			const FVector NewLocation =
				FMath::VInterpTo(BattalionLocation, TargetLocation, DeltaSeconds, 2.5f);
			SetActorLocation(NewLocation);
		}
	}
}

void ABattalionActor::TickFearAura(float DeltaSeconds)
{
	if (FearRadius <= 0.f || FearMoraleDamagePerSecond <= 0.f)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> AllBattalions;
	UGameplayStatics::GetAllActorsOfClass(World, ABattalionActor::StaticClass(), AllBattalions);

	for (AActor* Actor : AllBattalions)
	{
		ABattalionActor* Other = Cast<ABattalionActor>(Actor);
		if (!Other || Other == this)
		{
			continue;
		}

		// Do not scare allies – only enemies.
		if (UnitType && Other->UnitType && UnitType->Faction == Other->UnitType->Faction)
		{
			continue;
		}

		if (Other->bImmuneToFearAura)
		{
			continue;
		}

		const float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
		if (Dist <= FearRadius)
		{
			Other->CurrentMorale -= FearMoraleDamagePerSecond * DeltaSeconds;
			Other->CurrentMorale  = FMath::Max(0.f, Other->CurrentMorale);
		}
	}
}
