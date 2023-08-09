// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "DemoEnemy.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "DemoEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class YGD_TPS_DEMO_API ADemoEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ADemoEnemyAIController();

	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = AIBehaviorComponent, meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* EnemyBlackboardComponent;
	UPROPERTY(BlueprintReadWrite, Category = AIBehaviorComponent, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* EnemyBehaviorTreeComponent;

public:
	FORCEINLINE UBlackboardComponent* GetEnemyBlackboardComponent(){return EnemyBlackboardComponent;}
	FORCEINLINE UBehaviorTreeComponent* GetEnemyBehaviorTreeComponent()  { return EnemyBehaviorTreeComponent; }

};
