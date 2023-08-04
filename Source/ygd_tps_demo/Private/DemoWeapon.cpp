// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoWeapon.h"

ADemoWeapon::ADemoWeapon() :
	AmmoAmount(0),
	BulletMaxAmmo(0),
	ReloadTime(1.f),
	AutoFireInterval(0.1f),
	bCanFire(true),
	bReloading(false),
	WeaponFireMode(EFireMode::EFireMode_Auto)
{
	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetSimulatePhysics(false);

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetDropInfoWidget()->SetupAttachment(GetRootComponent());
	GetInteractAreaSphere()->SetupAttachment(GetRootComponent());
}

void ADemoWeapon::Tick(float DeltaTime)
{
	if (GetItemState() == EItemState::EItemState_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ADemoWeapon::FireTimer()
{
	if (bCanFire)
	{
		bCanFire = false;
		GetWorldTimerManager().SetTimer(
			AutoFireTimerHandle,
			this,
			&ADemoWeapon::ResetFireTimer,
			AutoFireInterval);
	}
}


void ADemoWeapon::ResetFireTimer()
{
	bCanFire = true;
}



void ADemoWeapon::SetWeaponFireMode(EFireMode NewWeaponFireMode)
{
	WeaponFireMode = NewWeaponFireMode;
}



void ADemoWeapon::DecrementAmmoAmount(int32 DecrementAmount)
{
	if (AmmoAmount < DecrementAmount) {}
	else{ AmmoAmount -= DecrementAmount;}
}

void ADemoWeapon::ReloadAmmo(int32 ReloadAmmoAmount)
{
	bReloading = true;
	
	FTimerDelegate ReloadDelegate;

	ReloadDelegate = FTimerDelegate::CreateUObject(this, &ADemoWeapon::CompleteReloadAmmo, ReloadAmmoAmount);

	GetWorldTimerManager().SetTimer(ReloadTimerHandle, ReloadDelegate, ReloadTime, false);

}

void ADemoWeapon::CompleteReloadAmmo(int32 ReloadAmmoAmount)
{
	AmmoAmount = ReloadAmmoAmount;
	bReloading = false;
}

void ADemoWeapon::SwitchItemProperty(EItemState State)
{
	switch (State)
	{
	case EItemState::EItemState_Drop:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetInteractAreaSphere()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		GetInteractAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		GetCollisionBox()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCollisionBox()->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		GetCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EItemState_Falling:
		UE_LOG(LogTemp, Log, TEXT("Weapon Falling"));
		bFalling = true;
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);

		GetInteractAreaSphere()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetInteractAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetCollisionBox()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EItemState_Equipped:
		// DropInfoWidget->SetVisibility(false);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetInteractAreaSphere()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetInteractAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetCollisionBox()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void ADemoWeapon::ItemStartFalling()
{

	SetItemState(EItemState::EItemState_Falling);

	//FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	//GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	//FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	//FVector MeshRight{ GetItemMesh()->GetRightVector() };

	//// Direction throw weapon
	//FVector ImpluseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);
	//float RandomRotation{ 30.f };
	//ImpluseDirection = ImpluseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	//ImpluseDirection *= 2000.f;

	//GetItemMesh()->AddImpulse(ImpluseDirection);

	GetWorldTimerManager().SetTimer(
		ItemFallingTimer,
		this,
		&ADemoWeapon::ItemStopFalling,
		ItemFallingTime);
	
}

void ADemoWeapon::ItemStopFalling()
{
	bFalling = true;

	SetItemState(EItemState::EItemState_Drop);
}
