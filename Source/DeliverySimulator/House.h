#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "House.generated.h"

class UStaticMeshComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPackageDelivered);

UCLASS()
class DELIVERYSIMULATOR_API AHouse : public AActor
{
	GENERATED_BODY()
	
public:	
	AHouse();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	FVector GetDeliveryPoint();
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* HouseMesh;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DeliveryPoint;
	
	FOnPackageDelivered OnPackageDeliveredDelegate;
};
