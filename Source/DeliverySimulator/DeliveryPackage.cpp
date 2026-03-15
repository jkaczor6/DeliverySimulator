#include "DeliveryPackage.h"

ADeliveryPackage::ADeliveryPackage()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box Mesh"));
	BoxMesh->SetupAttachment(RootComponent);
}

void ADeliveryPackage::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADeliveryPackage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

