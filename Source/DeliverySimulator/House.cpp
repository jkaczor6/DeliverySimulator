#include "House.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "PlayerCharacter.h"

AHouse::AHouse()
{
	PrimaryActorTick.bCanEverTick = true;
	
	HouseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("House Mesh"));
	SetRootComponent(HouseMesh);
	
	DeliveryPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Delivery Point"));
	DeliveryPoint->SetVisibility(false);
	DeliveryPoint->SetupAttachment(RootComponent);
}

void AHouse::BeginPlay()
{
	Super::BeginPlay();
	
	DeliveryPoint->OnComponentBeginOverlap.AddDynamic(this, &AHouse::OnBeginOverlap);
}

void AHouse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHouse::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		if (Player->HasActiveOrder && Player->IsHoldingPackage)
		{
			DeliveryPoint->SetVisibility(false);
			OnPackageDeliveredDelegate.Broadcast();		
		}
	}
}

FVector AHouse::GetDeliveryPoint()
{
	return DeliveryPoint->GetComponentLocation();
}

