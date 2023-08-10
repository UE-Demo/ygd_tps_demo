// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoEnemy.h"
#include "DemoEnemyAIController.h"
#include "DrawDebugHelpers.h"
#include "DemoCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
ADemoEnemy::ADemoEnemy():
	EnemyHealth(100.f),
	EnemyMaxHealth(100.f),

	InfoWidgetDisplayTime(4.f),

	bCanHitReact(true),
	HitReactTime(0.3f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HostileSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HostileSphere"));
	HostileSphere->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADemoEnemy::BeginPlay()
{
	Super::BeginPlay();

	HostileSphere->OnComponentBeginOverlap.AddDynamic(this, &ADemoEnemy::HostileSphereOverlap);
	HostileSphere->OnComponentEndOverlap.AddDynamic(this, &ADemoEnemy::HostileSphereEndOverlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), EnemyPatrolPoint);

	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		EnemyPatrolPoint2);

	DrawDebugSphere(GetWorld(), WorldPatrolPoint, 25.f, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), WorldPatrolPoint2, 25.f, 12, FColor::Red, true);

	EnemyController = Cast<ADemoEnemyAIController>(GetController());
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);

		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint2"),
			WorldPatrolPoint2);

		EnemyController->RunBehaviorTree(EnemyBehaviorTree);
	}

}

void ADemoEnemy::HostileSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<ADemoCharacter>(OtherActor);
	if (Character)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			TEXT("TargetActor"),
			Character);

		UE_LOG(LogTemp, Log, TEXT("HostileSphereOverlap"));
	}
}

void ADemoEnemy::HostileSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<ADemoCharacter>(OtherActor);
	if (Character)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			TEXT("TargetActor"),
			nullptr);

		UE_LOG(LogTemp, Log, TEXT("HostileSphereEndOverlap"));
	}
}

// Called every frame
void ADemoEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADemoEnemy::ShowInfoWidget_Implementation()
{
	GetWorldTimerManager().ClearTimer(InfoWidgetTimer);
	GetWorldTimerManager().SetTimer(InfoWidgetTimer, this, &ADemoEnemy::HideInfoWidget, InfoWidgetDisplayTime);
}

void ADemoEnemy::PlayEnemyHitMontage(FName MontageSection, float PlayRate)
{
	if (bCanHitReact)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(EnemyHitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(MontageSection, EnemyHitMontage);

			bCanHitReact = false;

			GetWorldTimerManager().SetTimer(EnemyHitReactTimer, this, &ADemoEnemy::ResetEnemyHitReactTimer, HitReactTime);
		}
	}
}

void ADemoEnemy::ResetEnemyHitReactTimer()
{
	bCanHitReact = true;
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