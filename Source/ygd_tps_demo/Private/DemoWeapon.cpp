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
}

void ADemoWeapon::Tick(float DeltaTime)
{
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
	if (AmmoAmount < DecrementAmount)
	{
		// do not decrement ammo amount
	}
	else
	{
		AmmoAmount -= DecrementAmount;
	}
}

void ADemoWeapon::ReloadAmmo(int32 ReloadAmmoAmount)
{
	bReloading = true;
	
	FTimerDelegate ReloadDelegate;
	//ReloadDelegate.BindUFunction(this, FName(TEXT("CompleteReloadAmmo")), ReloadAmmoAmount);

	ReloadDelegate = FTimerDelegate::CreateUObject(this, &ADemoWeapon::CompleteReloadAmmo, ReloadAmmoAmount);

	GetWorldTimerManager().SetTimer(ReloadTimerHandle, ReloadDelegate, ReloadTime, false);

}

void ADemoWeapon::CompleteReloadAmmo(int32 ReloadAmmoAmount)
{
	AmmoAmount = ReloadAmmoAmount;
	bReloading = false;
}
