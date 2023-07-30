// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoWeapon.h"

ADemoWeapon::ADemoWeapon() :
	AmmoAmount(0),
	BulletMaxAmmo(0),
	WeaponFireMode(EFireMode::EFireMode_Auto)
{
}

void ADemoWeapon::Tick(float DeltaTime)
{
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
	AmmoAmount = ReloadAmmoAmount;
}
