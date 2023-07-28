// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoItem.h"
#include "DemoCharacter.h"



// Sets default values
ADemoItem::ADemoItem():
	ItemName(FString("DefaultItemName")),
	ItemState(EItemState::EItemState_Drop),
	InteractAreaSphereRadius(150.f)
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

	InteractAreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractAreaSphere"));
	InteractAreaSphere->SetupAttachment(GetRootComponent());
	InteractAreaSphere->SetSphereRadius(InteractAreaSphereRadius);
#pragma endregion
}

// Called when the game starts or when spawned
void ADemoItem::BeginPlay()
{
	Super::BeginPlay();

	DropInfoWidget->SetVisibility(false);
	
	InteractAreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ADemoItem::OnInteractAreaStartOverlap);
	InteractAreaSphere->OnComponentEndOverlap.AddDynamic(this, &ADemoItem::OnInteractAreaeEndOverlap);

	SwitchItemProperty(ItemState);
}

// Called every frame
void ADemoItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADemoItem::OnInteractAreaStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ADemoCharacter* DemoCharacter = Cast<ADemoCharacter>(OtherActor);
		if (DemoCharacter)
		{
			DemoCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void ADemoItem::OnInteractAreaeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ADemoCharacter* DemoCharacter = Cast<ADemoCharacter>(OtherActor);
		if (DemoCharacter)
		{
			DemoCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

void ADemoItem::SwitchItemProperty(EItemState State)
{
	switch (State)
	{
	case EItemState::EItemState_Drop:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		InteractAreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		InteractAreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EItemState_Equipped:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		InteractAreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		InteractAreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ADemoItem::SetItemState(EItemState State)
{
	ItemState = State;
	SwitchItemProperty(State);
}


