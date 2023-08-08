// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoEnemy.h"

// Sets default values
ADemoEnemy::ADemoEnemy():
	EnemyHealth(100.f),
	EnemyMaxHealth(100.f),

	InfoWidgetDisplayTime(4.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADemoEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	
}

void ADemoEnemy::ShowInfoWidget_Implementation()
{
	GetWorldTimerManager().ClearTimer(InfoWidgetTimer);
	GetWorldTimerManager().SetTimer(InfoWidgetTimer, this, &ADemoEnemy::HideInfoWidget , InfoWidgetDisplayTime);
}

void ADemoEnemy::PlayEnemyHitMontage(FName MontageSection, float PlayRate)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(EnemyHitMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(MontageSection, EnemyHitMontage);
	}
}

// Called every frame
void ADemoEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Log, TEXT("Enemy Health: %f"), EnemyHealth);

}

// Called to bind functionality to input
void ADemoEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADemoEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if (BulletHitImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitImpactParticles, HitResult.Location, FRotator(0.f), true);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Enemy BulletHitImpactParticles lost."))
	}

	if (BulletHitSounds)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BulletHitSounds, GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Enemy BulletHitSounds lost."))
	}

	ShowInfoWidget();

	PlayEnemyHitMontage(FName("HitReact_1"));
}

float ADemoEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvenet, AController* EvenInstigator, AActor* DamageCauser)
{
	if (EnemyHealth - DamageAmount <= 0.f)
	{
		EnemyHealth = 0.f;
		EnemyDie();
	}
	else
	{
		EnemyHealth -= DamageAmount;
	}

	return DamageAmount;
}

void ADemoEnemy::EnemyDie()
{
	HideInfoWidget();
}