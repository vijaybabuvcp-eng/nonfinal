#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitTypeData.h"
#include "BattalionActor.generated.h"

class AHeroCommanderCharacter;

/** Very simple high-level order state for now. */
UENUM(BlueprintType)
enum class EBattalionOrder : uint8
{
    None,
    Move,
    Hold,
    Retreat
};

/**
 * Represents a single battalion on the battlefield.
 * Simulation is at battalion level, not per individual soldier.
 */
UCLASS()
class ROMANERAGAME_API ABattalionActor : public AActor
{
    GENERATED_BODY()

public:
    ABattalionActor();

    virtual void Tick(float DeltaSeconds) override;

    // -------- Data --------

    /** Data asset that defines stats/visuals for this battalion. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battalion")
    UUnitTypeData* UnitType;

    /** Current number of living soldiers. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battalion")
    int32 CurrentSoldiers;

    /** Current average morale (0–100). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battalion")
    float CurrentMorale;

    /** AI difficulty for this battalion (copied from UnitType by default). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    EUnitAIDifficulty AIDifficulty;

    // -------- General Bodyguard --------

    /** True if this battalion is the General's personal bodyguard. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bodyguard")
    bool bIsGeneralBodyguard = false;

    /** Hero that this bodyguard protects. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bodyguard")
    AHeroCommanderCharacter* ProtectedHero;

    /** Preferred distance to stay from the hero (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bodyguard")
    float DesiredFollowRadius;

    /** Hard leash distance – if exceeded, snap back closer to the hero (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bodyguard")
    float HardLeashRadius;

    // -------- Morale & Fear Aura --------

    /** If true, this battalion never routes from morale loss. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale")
    bool bMoraleUnbreakable;

    /** If true, this battalion applies a fear aura to nearby enemies. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale")
    bool bCausesFearAura;

    /** If true, this battalion ignores fear auras. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale")
    bool bImmuneToFearAura;

    /** Radius of fear aura if enabled (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale")
    float FearRadius;

    /** Morale damage per second inside fear aura. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Morale")
    float FearMoraleDamagePerSecond;

    // -------- Commands (for Command Mode later) --------

    UFUNCTION(BlueprintCallable, Category = "Commands")
    void Command_MoveTo(const FVector& WorldLocation);

    UFUNCTION(BlueprintCallable, Category = "Commands")
    void Command_HoldPosition();

    UFUNCTION(BlueprintCallable, Category = "Commands")
    void Command_Retreat();

    /** Called by spawn/hero logic to mark this as the General's bodyguard. */
    void InitializeAsGeneralBodyguard(AHeroCommanderCharacter* InHero, UUnitTypeData* InUnitType);

protected:
    virtual void BeginPlay() override;

    FVector         LastCommandLocation;
    EBattalionOrder CurrentOrder;

    void TickBodyguardFollow(float DeltaSeconds);
    void TickFearAura(float DeltaSeconds);
};
