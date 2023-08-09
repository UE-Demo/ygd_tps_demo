// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoEnemy.h"

#include "Animation/AnimInstance.h"
#include "DemoEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class YGD_TPS_DEMO_API UDemoEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ADemoEnemy* Enemy;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float EnemySpeed;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);
};
