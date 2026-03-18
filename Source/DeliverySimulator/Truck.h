#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Truck.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class DELIVERYSIMULATOR_API ATruck : public AActor
{
	GENERATED_BODY()
	
public:	
	ATruck();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnCabinOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnCabinOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnTrunkOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnTrunkOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TruckMesh;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CabinPlayerDetector;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TrunkPlayerDetector;
	
	bool HasPackageAtTrunk = false;
	bool PlayerOverlappingWithTrunk = false;
	bool PlayerOverlappingWithCabin = false;
};
