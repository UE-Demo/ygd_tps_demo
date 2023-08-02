// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoItem.h"
#include "DemoCharacter.h"



// Sets default values
ADemoItem::ADemoItem() :
	ItemName(FString("DefaultItemName")),
	ItemState(EItemState::EItemState_Drop),
	bFalling(false),
	ItemFallingTime(1.f),
	InteractAreaSphereRadius(150.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region initItem



	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));

	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,
		ECollisionResponse::ECR_Block);


	DropInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DropInfoWidget"));

	DropInfoWidget->SetRelativeLocation(FVector(0, 0, 100));

	InteractAreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractAreaSphere"));

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

void ADemoItem::SetItemState(EItemState State)
{
	ItemState = State;
	SwitchItemProperty(State);
}

void ADemoItem::SwitchItemProperty(EItemState State)
{
	switch (State)
	{
	case EItemState::EItemState_Drop:
		break;

	case EItemState::EItemState_Falling:
		break;

	case EItemState::EItemState_Equipped:
		break;
	}
}

void ADemoItem::ItemStartFalling()
{
	SetItemState(EItemState::EItemState_Falling);

	GetWorldTimerManager().SetTimer(
		ItemFallingTimer,
		this,
		&ADemoItem::ItemStopFalling,
		ItemFallingTime);
}

void ADemoItem::ItemStopFalling()
{
	bFalling = true;

	SetItemState(EItemState::EItemState_Drop);
}




