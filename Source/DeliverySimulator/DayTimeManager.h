#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayTimeManager.generated.h"

class UDirectionalLightComponent;
class APlayerCharacter;

UCLASS()
class DELIVERYSIMULATOR_API ADayTimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ADayTimeManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void EndDayAndReset();

	UPROPERTY(EditAnywhere, Category = "Time")
	float DayLength = 600.0f;
	UPROPERTY(VisibleAnywhere, Category = "Time")
	float TimeOfDay = 0.0f;
	UPROPERTY(VisibleAnywhere, Category = "Time")
	bool bIsDayFinished = false;
	
	UPROPERTY(EditAnywhere, Category = "Time")
	UDirectionalLightComponent* DirectionalLight;
	
	APlayerCharacter* Player;
	FString CurrentHour;
};
