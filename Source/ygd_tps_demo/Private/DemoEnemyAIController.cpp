// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoEnemyAIController.h"

ADemoEnemyAIController::ADemoEnemyAIController()
{
	EnemyBlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));

	EnemyBehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void ADemoEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn == nullptr) return;

	ADemoEnemy* Enemy = Cast<ADemoEnemy>(InPawn);

	if (Enemy)
	{
		if (Enemy->GetEnemyBehaviorTree())
		{
			EnemyBlackboardComponent->InitializeBlackboard(*(Enemy->GetEnemyBehaviorTree()->BlackboardAsset));
		}
	}

}
