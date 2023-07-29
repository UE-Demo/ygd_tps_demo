// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoWeapon.h"

ADemoWeapon::ADemoWeapon():
	AmmoAmount(0)
{
}

void ADemoWeapon::Tick(float DeltaTime)
{
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
