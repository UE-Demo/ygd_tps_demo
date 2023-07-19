// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoItem.h"



// Sets default values
ADemoItem::ADemoItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region initItem

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,
		ECollisionResponse::ECR_Block);


	DropInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DropInfoWidget"));
	DropInfoWidget->SetupAttachment(GetRootComponent());
	DropInfoWidget->SetRelativeLocation(FVector(0, 0, 100));

#pragma endregion
}

// Called when the game starts or when spawned
void ADemoItem::BeginPlay()
{
	Super::BeginPlay();

	DropInfoWidget->SetVisibility(false);
	
}

// Called every frame
void ADemoItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

