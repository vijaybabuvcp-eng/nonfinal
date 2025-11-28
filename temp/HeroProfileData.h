// HeroProfileData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HeroProfileData.generated.h"

class UUnitTypeData;
class UAnimInstance;
class USkeletalMesh;

UENUM(BlueprintType)
enum class EHeroArchetype : uint8
{
	General,
	Commander1,
	Commander2
};

UENUM(BlueprintType)
enum class EHeroWeaponCategory : uint8
{
	SwordShield,
	LongSword,
	SpearShield,
	TwoHandedAxe,
	Pike
};

UENUM(BlueprintType)
enum class EHeroRoleTag : uint8
{
	Tank,
	DPS,
	Controller
};

UCLASS(BlueprintType)
class ROMANERAGAME_API UHeroProfileData : public UDataAsset
{
	GENERATED_BODY()

public:
	// --- Identity -------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero")
	EHeroArchetype Archetype = EHeroArchetype::General;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero")
	EHeroRoleTag RoleTag = EHeroRoleTag::Tank;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero")
	EHeroWeaponCategory WeaponCategory = EHeroWeaponCategory::SwordShield;

	// --- Core stats -----------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxStamina = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Poise = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float DamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MoveSpeedMultiplier = 1.f;

	// --- Visuals --------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TSoftObjectPtr<USkeletalMesh> HeroMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TSoftClassPtr<UAnimInstance> AnimBlueprint;

	// --- Personal bodyguard configuration ------------------------------------
	// Only set this to true for General profiles. ArmyManager will use this
	// to spawn an elite, AI-only bodyguard battalion for the General.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
	bool bHasPersonalBodyguard = false;

	// Which unit type is used as the General's personal bodyguard.
	// This unit type will usually be marked as a bodyguard template and
	// not selectable in the normal army builder.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard",
		meta = (EditCondition = "bHasPersonalBodyguard"))
	TSoftObjectPtr<UUnitTypeData> BodyguardUnitType;
};
