#include "Truck.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"

ATruck::ATruck()
{
	PrimaryActorTick.bCanEverTick = true;

	TruckMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Truck Mesh"));
	SetRootComponent(TruckMesh);
	
	CabinPlayerDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("Cabin Player Detector"));
	CabinPlayerDetector->SetupAttachment(RootComponent);
	
	TrunkPlayerDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("Trunk Player Detector"));
	TrunkPlayerDetector->SetupAttachment(RootComponent);
}

void ATruck::BeginPlay()
{
	Super::BeginPlay();
	
	CabinPlayerDetector->OnComponentBeginOverlap.AddDynamic(this, &ATruck::OnCabinOverlapBegin);
	CabinPlayerDetector->OnComponentEndOverlap.AddDynamic(this, &ATruck::OnCabinOverlapEnd);
	TrunkPlayerDetector->OnComponentBeginOverlap.AddDynamic(this, &ATruck::OnTrunkOverlapBegin);
	TrunkPlayerDetector->OnComponentEndOverlap.AddDynamic(this, &ATruck::OnTrunkOverlapEnd);
}

void ATruck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATruck::OnCabinOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Player can enter cabin")));
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		PlayerOverlappingWithCabin = true;
	}
}

void ATruck::OnCabinOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerOverlappingWithCabin = false;
}

void ATruck::OnTrunkOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Player can leave package at trunk")));
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		PlayerOverlappingWithTrunk = true;
		if (Player->IsHoldingPackage)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Package hidden at trunk")));
		}
	}
}

void ATruck::OnTrunkOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerOverlappingWithTrunk = false;
}

