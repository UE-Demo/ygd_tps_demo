// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoAmmo.h"

ADemoAmmo::ADemoAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);
}
