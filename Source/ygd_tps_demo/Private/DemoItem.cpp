// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoItem.h"
#include "DemoCharacter.h"



// Sets default values
ADemoItem::ADemoItem():
	ItemName(FString("DefaultItemName")),
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


