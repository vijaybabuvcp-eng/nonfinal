#include "HeroCommanderCharacter.h"

#include "BattalionActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UnitTypeData.h"

AHeroCommanderCharacter::AHeroCommanderCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Movement defaults: slightly slower than typical.
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = 450.f;
    }
}

void AHeroCommanderCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Optionally apply movement scaling from the hero profile.
    if (HeroProfile)
    {
        if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
        {
            MoveComp->MaxWalkSpeed *= HeroProfile->MoveSpeedMultiplier;
        }
    }

    // Spawn the bodyguard after profile application.
    TrySpawnBodyguardBattalion();
}

void AHeroCommanderCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UpdateBodyguardFollow(DeltaSeconds);
}

void AHeroCommanderCharacter::TrySpawnBodyguardBattalion()
{
    if (!bAutoSpawnBodyguardFromProfile)
    {
        return;
    }

    // Need a valid profile and a configured bodyguard unit.
    if (!HeroProfile || !HeroProfile->HasPersonalBodyguard || !HeroProfile->BodyguardUnitType)
    {
        return;
    }

    // Already have one.
    if (BodyguardBattalion)
    {
        return;
    }

    if (!BodyguardBattalionClass)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("HeroCommanderCharacter %s has a personal bodyguard profile but no BodyguardBattalionClass set."),
            *GetName());
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Resolve the soft reference into an actual UUnitTypeData*.
    UUnitTypeData* BodyguardUnitType = HeroProfile->BodyguardUnitType.LoadSynchronous();
    if (!BodyguardUnitType)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("HeroCommanderCharacter %s could not load bodyguard unit type asset."),
            *GetName());
        return;
    }

    const FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(BodyguardSpawnOffset);
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

    // Initialise the battalion as the hero's elite personal guard.
    BodyguardBattalion->InitializeAsGeneralBodyguard(this, BodyguardUnitType);

    UE_LOG(LogTemp, Log,
        TEXT("HeroCommanderCharacter %s spawned bodyguard battalion %s using unit type %s."),
        *GetName(),
        *BodyguardBattalion->GetName(),
        *BodyguardUnitType->GetName());
}

void AHeroCommanderCharacter::UpdateBodyguardFollow(float DeltaSeconds)
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
        const FVector HeroForward = GetActorForwardVector();
        const FVector TargetLocation = HeroLocation - HeroForward * DesiredBodyguardFollowDistance;

        // ABattalionActor is expected to obey this order.
        BodyguardBattalion->Command_MoveTo(TargetLocation);
    }
}
