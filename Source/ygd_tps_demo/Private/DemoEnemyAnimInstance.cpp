// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoEnemyAnimInstance.h"

void UDemoEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<ADemoEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		FVector EnemyVelocity{ Enemy->GetVelocity() };
		EnemyVelocity.Z = 0.f;
		EnemySpeed = EnemyVelocity.Size();

		EnemyIsDead = Enemy->GetIsDead();
	}
	else { UE_LOG(LogTemp, Warning, TEXT("No EnemyBP binding to EnemyAnimInstance")); }
}
