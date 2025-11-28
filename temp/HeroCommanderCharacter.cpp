// HeroCommanderCharacter.cpp

#include "HeroCommanderCharacter.h"

#include "StaminaComponent.h"
#include "CombatComponent.h"
#include "HeroProfileData.h"
#include "UnitTypeData.h"
#include "BattalionActor.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

// --------------------------------------------------------
// Constructor
// --------------------------------------------------------

AHeroCommanderCharacter::AHeroCommanderCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Components
	StaminaComp = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComp"));
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));

	// Bodyguard defaults
	bAutoSpawnBodyguardFromProfile = true;
	BodyguardSpawnOffset = FVector(800.f, 0.f, 0.f); // ~8m in front by default
	DesiredBodyguardFollowDistance = 900.f;                    // keep fairly close
	BodyguardBattalion = nullptr;
}

// --------------------------------------------------------
// BeginPlay / Tick
// --------------------------------------------------------

void AHeroCommanderCharacter::BeginPlay()
{
	Super::BeginPlay();

	ApplyHeroProfileToMovement();

	if (bAutoSpawnBodyguardFromProfile)
	{
		TrySpawnBodyguardBattalion();
	}
}

void AHeroCommanderCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateBodyguardFollow(DeltaSeconds);
}

// --------------------------------------------------------
// Hero profile → movement
// --------------------------------------------------------

void AHeroCommanderCharacter::ApplyHeroProfileToMovement()
{
	if (!HeroProfile)
	{
		return;
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		// HeroProfile should expose a MoveSpeedMultiplier field (1.0 = default).
		const float MoveScale = FMath::Max(0.1f, HeroProfile->MoveSpeedMultiplier);
		MoveComp->MaxWalkSpeed *= MoveScale;
	}
}

// --------------------------------------------------------
// Bodyguard spawning
// --------------------------------------------------------

void AHeroCommanderCharacter::TrySpawnBodyguardBattalion()
{
	// Only the authoritative instance (server / standalone) should spawn battalions.
	if (!HasAuthority())
	{
		return;
	}

	if (BodyguardBattalion != nullptr)
	{
		return; // already spawned
	}

	if (!HeroProfile || !HeroProfile->bHasPersonalBodyguard || !HeroProfile->BodyguardUnitType)
	{
		return; // this hero doesn't use a personal bodyguard
	}

	if (!BodyguardBattalionClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("HeroCommanderCharacter %s wants a bodyguard but BodyguardBattalionClass is not set."),
			*GetName());
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Compute spawn transform based on hero location + local offset.
	const FVector SpawnLocation =
		GetActorLocation()
		+ GetActorForwardVector() * BodyguardSpawnOffset.X
		+ GetActorRightVector() * BodyguardSpawnOffset.Y
		+ GetActorUpVector() * BodyguardSpawnOffset.Z;

	const FRotator SpawnRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	BodyguardBattalion = World->SpawnActor<ABattalionActor>(
		BodyguardBattalionClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	if (!BodyguardBattalion)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("HeroCommanderCharacter %s failed to spawn bodyguard battalion."),
			*GetName());
		return;
	}

	// Initialise from the unit template.  ABattalionActor should already know how
	// to read things like IsGeneralBodyguardTemplate / MoraleUnbreakable from
	// the UUnitTypeData asset.
	BodyguardBattalion->InitializeFromUnitType(HeroProfile->BodyguardUnitType);

	UE_LOG(LogTemp, Log,
		TEXT("HeroCommanderCharacter %s spawned bodyguard battalion %s using unit type %s."),
		*GetName(),
		*BodyguardBattalion->GetName(),
		*HeroProfile->BodyguardUnitType->GetName());
}

// --------------------------------------------------------
// Bodyguard follow behaviour
// --------------------------------------------------------

void AHeroCommanderCharacter::UpdateBodyguardFollow(float /*DeltaSeconds*/)
{
	if (!BodyguardBattalion)
	{
		return;
	}

	// Keep the bodyguard fairly close and slightly behind the hero.
	const FVector HeroLocation = GetActorLocation();
	const FVector BattalionLocation = BodyguardBattalion->GetActorLocation();

	const float DistSq = FVector::DistSquared2D(HeroLocation, BattalionLocation);
	const float DesiredDistSq = DesiredBodyguardFollowDistance * DesiredBodyguardFollowDistance;

	if (DistSq > DesiredDistSq)
	{
		// Target a point behind the hero at the desired distance.
		const FVector TargetLocation =
			HeroLocation - GetActorForwardVector() * DesiredBodyguardFollowDistance;

		// ABattalionActor is expected to expose this order method.
		BodyguardBattalion->Command_MoveTo(TargetLocation);
	}
}
