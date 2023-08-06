// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DemoBulletHitInterface.h"

// Assets
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "DemoEnemy.generated.h"

UCLASS()
class YGD_TPS_DEMO_API ADemoEnemy : public ACharacter, public IDemoBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemoEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Assets
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* BulletHitImpactParticles;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USoundCue* BulletHitSounds;
#pragma endregion


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult) override;
};
