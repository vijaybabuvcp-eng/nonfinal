// HeroCommanderCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeroCommanderCharacter.generated.h"

class UStaminaComponent;
class UCombatComponent;
class UHeroProfileData;
class UUnitTypeData;
class ABattalionActor;

/**
 * Base hero class used for General / Commander 1 / Commander 2.
 * Handles stamina, combat, and (for the General) spawning / owning
 * a personal bodyguard battalion.
 */
UCLASS()
class ROMANERAGAME_API AHeroCommanderCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHeroCommanderCharacter();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* =====================  Components  ===================== */

	/** Stamina system shared by all heroes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaminaComponent* StaminaComp;

	/** Melee combat / hit detection system. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCombatComponent* CombatComp;

	/* =====================  Hero Data  ===================== */

	/** Data asset describing stats, visuals, and bodyguard config. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero")
	UHeroProfileData* HeroProfile;

	/* =====================  Bodyguard  ===================== */

	/** Whether this hero should auto‑spawn a personal bodyguard from the profile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
	bool bAutoSpawnBodyguardFromProfile;

	/** Battalion actor class to use when spawning the personal bodyguard. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
	TSubclassOf<ABattalionActor> BodyguardBattalionClass;

	/** Offset (in hero local space) used when spawning the bodyguard initially. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
	FVector BodyguardSpawnOffset;

	/** Desired follow distance (2D) the bodyguard tries to maintain from the hero. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
	float DesiredBodyguardFollowDistance;

	/** Runtime instance of the personal bodyguard (if any). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bodyguard")
	ABattalionActor* BodyguardBattalion;

	/** Spawns the bodyguard battalion based on HeroProfile, if configured. */
	void TrySpawnBodyguardBattalion();

	/** Issues follow / reposition orders to keep the bodyguard near the hero. */
	void UpdateBodyguardFollow(float DeltaSeconds);

	/** Applies basic movement tuning from the HeroProfile (e.g. move speed). */
	void ApplyHeroProfileToMovement();

public:

	/* =====================  Accessors  ===================== */

	UFUNCTION(BlueprintCallable, Category = "Hero")
	UHeroProfileData* GetHeroProfile() const { return HeroProfile; }

	UFUNCTION(BlueprintCallable, Category = "Bodyguard")
	ABattalionActor* GetBodyguardBattalion() const { return BodyguardBattalion; }
};
