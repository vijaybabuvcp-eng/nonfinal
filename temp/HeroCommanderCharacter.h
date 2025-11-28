#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeroProfileData.h"
#include "HeroCommanderCharacter.generated.h"

class ABattalionActor;

/**
 * Player‑controlled hero commander.
 * Reads stats/visuals from UHeroProfileData and owns the personal bodyguard
 * battalion for this hero.
 */
UCLASS()
class ROMANERAGAME_API AHeroCommanderCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AHeroCommanderCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    /** Profile holding designer-authored stats + visuals. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero")
    UHeroProfileData* HeroProfile;

    /** Automatically spawn the personal bodyguard battalion on BeginPlay. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
    bool bAutoSpawnBodyguardFromProfile = true;

    /** Class of the battalion actor used for the personal bodyguard. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
    TSubclassOf<ABattalionActor> BodyguardBattalionClass;

    /** Offset from the hero where the bodyguard should spawn. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
    FVector BodyguardSpawnOffset = FVector(300.f, 0.f, 0.f);

    /** How closely the bodyguard should try to follow the hero. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bodyguard")
    float DesiredBodyguardFollowDistance = 600.f;

    /** Runtime spawned instance of the bodyguard battalion. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bodyguard")
    ABattalionActor* BodyguardBattalion = nullptr;

protected:
    /** Attempt to spawn the personal bodyguard from the hero profile, if valid. */
    void TrySpawnBodyguardBattalion();

    /** Per-frame follow logic for the bodyguard. */
    void UpdateBodyguardFollow(float DeltaSeconds);
};
