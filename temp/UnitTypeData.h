#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UnitTypeData.generated.h"

/** Broad unit category (for UI / filtering). */
UENUM(BlueprintType)
enum class EUnitClass : uint8
{
    Infantry,
    Ranged,
    Cavalry,
    Special
};

/** Battlefield role – helps AI & UI understand what the unit does. */
UENUM(BlueprintType)
enum class EUnitRole : uint8
{
    Line,
    Skirmish,
    Shock,
    Support
};

/** Which faction this unit belongs to. */
UENUM(BlueprintType)
enum class EUnitFaction : uint8
{
    Rome,
    Egypt,
    Carthage,
    Sparta,
    Gaul,
    Britannia,
    Germania,
    Seleucid
};

/** Baseline AI difficulty / behavior profile for the unit. */
UENUM(BlueprintType)
enum class EUnitAIDifficulty : uint8
{
    Easy   UMETA(DisplayName = "Easy"),
    Normal UMETA(DisplayName = "Normal"),
    Hard   UMETA(DisplayName = "Hard"),
    Elite  UMETA(DisplayName = "Elite")
};

/**
 * Data describing a single battalion type (Hastati, Praetorians, etc.).
 * Used by the army builder and battalion spawning.
 */
UCLASS(BlueprintType)
class ROMANERAGAME_API UUnitTypeData : public UDataAsset
{
    GENERATED_BODY()

public:
    UUnitTypeData();

    // -------- Unit --------

    /** Stable ID used by save/load and army builder. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit")
    FName UnitId;

    /** Localised display name. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit")
    FText DisplayName;

    /** Broad class (Infantry / Ranged / Cavalry / Special). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit")
    EUnitClass UnitClass = EUnitClass::Infantry;

    /** Battlefield role (Line / Skirmish / Shock / Support). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit")
    EUnitRole UnitRole = EUnitRole::Line;

    /** Which faction this unit belongs to (for filtering in army builder). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit")
    EUnitFaction Faction = EUnitFaction::Rome;

    // -------- Stats --------

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    int32 MaxSoldiers = 200;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float MaxHealthPerSoldier = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float Attack = 50.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float Defense = 50.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float Armor = 50.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float BaseMorale = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float MoveSpeed = 600.f;

    // -------- Economy --------

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Economy")
    int32 RecruitmentCost = 100;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Economy")
    int32 UpkeepCost = 10;

    // -------- Visual --------

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    TSoftObjectPtr<USkeletalMesh> SoldierMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    TSoftClassPtr<UAnimInstance> AnimBlueprint;

    // -------- Army Builder --------

    /** If false, this unit will not appear as a selectable option in the army builder. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Army Builder")
    bool bSelectableInArmyBuilder = true;

    // -------- Special / Bodyguard & Fear Aura --------

    /** True if this template is intended to be used ONLY as the General’s bodyguard battalion. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Special")
    bool bIsGeneralBodyguardTemplate = false;

    /** If true, this unit never breaks from morale (used for elite bodyguards). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Special")
    bool bMoraleUnbreakable = false;

    /** If true, this battalion applies a fear aura to nearby enemies. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Special")
    bool bCausesFearAura = false;

    /** If true, this battalion ignores all fear auras. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Special")
    bool bImmuneToFearAura = false;

    /** Baseline AI difficulty for this unit. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Special")
    EUnitAIDifficulty BaseAIDifficulty = EUnitAIDifficulty::Normal;

    /** Radius of the fear aura, if enabled (in cm). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Special")
    float FearRadius = 800.f;

    /** Morale damage per second applied to enemies inside the fear aura. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Special")
    float FearMoraleDamagePerSecond = 8.f;
};
