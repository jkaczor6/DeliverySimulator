#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryPackage.generated.h"

class UStaticMeshComponent;

UCLASS()
class DELIVERYSIMULATOR_API ADeliveryPackage : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeliveryPackage();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* BoxMesh;
};
