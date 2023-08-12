// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoEnemy.h"
#include "DemoEnemyAIController.h"
#include "DrawDebugHelpers.h"
#include "DemoCharacter.h"
#include "DemoWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
ADemoEnemy::ADemoEnemy():
	EnemyHealth(100.f),
	EnemyMaxHealth(100.f),

	InfoWidgetDisplayTime(4.f),

	bCanHitReact(true),
	HitReactTime(0.3f),

	bIsStunned(false),
	StunTime(1.5f)
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

	EnemyEquipWeapon(SpawnEnemyDefaultWeapon());
}

void ADemoEnemy::HostileSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<ADemoCharacter>(OtherActor);
	if (Character)
	{
		TargetActor = Cast<AActor>(OtherActor);
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			TEXT("TargetActor"),
			Character);
	}
}

void ADemoEnemy::HostileSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<ADemoCharacter>(OtherActor);
	if (Character)
	{
		TargetActor = nullptr;
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			TEXT("TargetActor"),
			nullptr);
	}
}

ADemoWeapon* ADemoEnemy::SpawnEnemyDefaultWeapon()
{
	if (EnemyDefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<ADemoWeapon>(EnemyDefaultWeaponClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Not DefaultWeaponClass"));
		return nullptr;
	}
}

void ADemoEnemy::EnemyEquipWeapon(ADemoWeapon* WeaponToEquip)
{
	const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (RightHandSocket)
	{
		RightHandSocket->AttachActor(WeaponToEquip, GetMesh());
	}

	EnemyEquippedWeapon = WeaponToEquip;
	EnemyEquippedWeapon->SetItemState(EItemState::EItemState_Equipped);
}

void ADemoEnemy::EnemyWeaponFire()
{
	if (TargetActor == nullptr) return;
	if (EnemyEquippedWeapon->GetAmmoAmount() <= 0) return ;

	EnemyEquippedWeapon->FireTimer();

	const USkeletalMeshSocket* BarrelSocket =
		EnemyEquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	const FTransform BarrelSocketTransform = BarrelSocket->GetSocketTransform(EnemyEquippedWeapon->GetItemMesh());

	const FVector Start = BarrelSocketTransform.GetLocation();
	const FVector Direction{ (TargetActor->GetActorLocation() - Start).GetSafeNormal()};
	const FVector End{ Start + Direction*3000.f };
	const FVector EndIfNotHit{ Start + Direction * 3000.f };

	FHitResult EnemyHitResult;
	GetWorld()->LineTraceSingleByChannel(
		EnemyHitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	// ADemoCharacter* TargetPlayer;
	if (EnemyHitResult.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyHitResult.bBlockingHit"));
		AActor* BlockingActor = EnemyHitResult.GetActor();
		if (BlockingActor)
		{
			DrawDebugSphere(GetWorld(), EnemyHitResult.Location, 10.f, 12, FColor::Blue, true);
			UE_LOG(LogTemp, Warning, TEXT("BlockingActor : %s"), *BlockingActor->GetName());
			UE_LOG(LogTemp, Warning, TEXT("BlockingActor Location : %s"), *BlockingActor->GetActorLocation().ToString());
			if (ADemoCharacter* HitEnemy = Cast<ADemoCharacter>(BlockingActor))
			{
				UE_LOG(LogTemp, Warning, TEXT("HitEnemy"));
				//UGameplayStatics::ApplyDamage(BlockingActor, EnemyEquippedWeapon->GetWeaponDamage(), GetController(), this, UDamageType::StaticClass());
			}
		}
	}

	// Ballistic effects if not hit
	if (EnemyEquippedWeapon->GetWeaponBeamParticles())
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			EnemyEquippedWeapon->GetWeaponBeamParticles(),
			BarrelSocketTransform);

		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), EndIfNotHit);
		}
	}

	// Gun shot sounds
	if (EnemyEquippedWeapon->GetWeaponSounds())
	{
		UGameplayStatics::PlaySound2D(this, EnemyEquippedWeapon->GetWeaponSounds());
	}

	// Consume Ammo in weapon bullet
	if (EnemyEquippedWeapon)
	{
		EnemyEquippedWeapon->DecrementAmmoAmount(1);
	}
}

// Called every frame
void ADemoEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugSphere(GetWorld(), GetActorLocation(), 25.f, 12, FColor::Red, false);

	// Face to TargetActor
	if (TargetActor)
	{
		FVector TargetLocation = TargetActor->GetActorLocation();
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
		SetActorRotation(NewRotation);
	}

	if (!bIsStunned && !EnemyEquippedWeapon->GetReloading() && EnemyEquippedWeapon->GetCanFire())
	{
		EnemyWeaponFire();
	}
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

void ADemoEnemy::SetStunned()
{
	bIsStunned = true;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Stuned"), true);
	}
	GetWorldTimerManager().SetTimer(EnemyStunTimer, this, &ADemoEnemy::ResetStunned, StunTime);
}

void ADemoEnemy::ResetStunned()
{
	bIsStunned = false;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Stuned"), false);
	}
	
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

	SetStunned();
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