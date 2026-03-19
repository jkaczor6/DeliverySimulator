#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Truck.generated.h"

class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;
class UFloatingPawnMovement;
class UInputMappingContext;
class UInputAction;
class APlayerCharacter;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class DELIVERYSIMULATOR_API ATruck : public APawn
{
	GENERATED_BODY()
	
public:	
	ATruck();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	
	UFUNCTION()
	void OnCabinOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnCabinOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnTrunkOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnTrunkOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void ExitInput();
	void CameraOrbitInput(const FInputActionValue& Value);
	void SteerInput(const FInputActionValue& Value);
	void SteerReleased(const FInputActionValue& Value);
	void ThrottleInput(const FInputActionValue& Value);
	void ThrottleReleased(const FInputActionValue& Value);
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TruckMesh;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CabinPlayerDetector;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TrunkPlayerDetector;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* ExitPoint;
	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* MovementComponent;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* TruckIMC;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* CameraOrbitAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ExitAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SteerAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ThrottleAction;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float MaxSpeed = 1000.0f;
	UPROPERTY(EditAnywhere, Category="Movement")
	float Acceleration = 2000.0f;
	UPROPERTY(EditAnywhere, Category="Movement")
	float TurnSpeed = 100.0f;
	UPROPERTY(EditAnywhere, Category="Movement")
	float Friction =  3.0f;
	
	float CurrentSpeed = 0.0f;
	float CurrentThrottle = 0.0f;
	float CurrentSteer = 0.0f;
	
	APlayerCharacter* PlayerCharacter;
	
	bool HasPackageAtTrunk = false;
	bool PlayerOverlappingWithTrunk = false;
	bool PlayerOverlappingWithCabin = false;
};
