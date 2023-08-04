// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoAmmo.h"

ADemoAmmo::ADemoAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);


	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetDropInfoWidget()->SetupAttachment(GetRootComponent());
	GetInteractAreaSphere()->SetupAttachment(GetRootComponent());
}

void ADemoAmmo::BeginPlay()
{
	Super::BeginPlay();
}

void ADemoAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
