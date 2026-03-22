#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Warehouse.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class DELIVERYSIMULATOR_API AWarehouse : public AActor
{
	GENERATED_BODY()
	
public:	
	AWarehouse();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void PlayerDetectorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WarehouseMesh;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* PlayerDetector;
	
	bool PlayerOverlapping;
};
