// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DemoBulletHitInterface.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Assets
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "BehaviorTree/BehaviorTree.h"

#include "DemoEnemy.generated.h"

UCLASS()
class YGD_TPS_DEMO_API ADemoEnemy : public ACharacter, public IDemoBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemoEnemy();

#pragma region EnemyStatus

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = EnemyStatus, meta = (AllowPrivateAccess = "true"))
		float EnemyHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = EnemyStatus, meta = (AllowPrivateAccess = "true"))
		float EnemyMaxHealth;

	void EnemyDie();

#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Assets
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = HitAssets, meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* BulletHitImpactParticles;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = HitAssets, meta = (AllowPrivateAccess = "true"))
		class USoundCue* BulletHitSounds;
#pragma endregion

#pragma region InfoWidget
protected:

	bool bShowInfoWidget;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = InfoWidget, meta = (AllowPrivateAccess = "true"))
	float InfoWidgetDisplayTime;

	FTimerHandle InfoWidgetTimer;

	UFUNCTION(BlueprintNativeEvent)
	void ShowInfoWidget();
	void ShowInfoWidget_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideInfoWidget();

#pragma endregion

#pragma region Anim
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = HitReact, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* EnemyHitMontage;

	void PlayEnemyHitMontage(FName MontageSection, float PlayRate = 1.f);

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = HitReact, meta = (AllowPrivateAccess = "true"))
	bool bCanHitReact;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = HitReact, meta = (AllowPrivateAccess = "true"))
	float HitReactTime;

	FTimerHandle EnemyHitReactTimer;

	void ResetEnemyHitReactTimer();
#pragma endregion

#pragma region EnemyAI
protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = AIBehavior, meta = (AllowPrivateAccess = "true"))
		class UBehaviorTree* EnemyBehaviorTree;
public:
	FORCEINLINE UBehaviorTree* GetEnemyBehaviorTree() const { return EnemyBehaviorTree; }

protected:

	/* Point for enemy to move to. */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = AIBehavior, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector EnemyPatrolPoint;
#pragma endregion


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvenet, AController* EvenInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetHealthPercent() const { return EnemyHealth / EnemyMaxHealth; }

	FORCEINLINE bool IsShowInfoWidget() const { return bShowInfoWidget; }
	FORCEINLINE void SetShowInfoWidget(bool ShouldShowInfoWidget) {  bShowInfoWidget = ShouldShowInfoWidget; }
};
