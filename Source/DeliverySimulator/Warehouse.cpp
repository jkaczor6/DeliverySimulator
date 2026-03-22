#include "Warehouse.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"

AWarehouse::AWarehouse()
{
	PrimaryActorTick.bCanEverTick = true;

	WarehouseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Warehouse Mesh"));
	SetRootComponent(WarehouseMesh);
	
	PlayerDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("Player Detector"));
	PlayerDetector->SetupAttachment(RootComponent);
}

void AWarehouse::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerDetector->OnComponentBeginOverlap.AddDynamic(this, &AWarehouse::PlayerDetectorOverlapBegin);
}

void AWarehouse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWarehouse::PlayerDetectorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerOverlapping = true;
	}
}

