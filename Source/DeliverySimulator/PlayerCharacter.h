#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USceneComponent;
class ADeliveryPackage;
class AHouse;
class ATruck;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionValue;

UCLASS()
class DELIVERYSIMULATOR_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void InteractInput(const FInputActionValue& Value);
	void SpawnDebugBoxInput(const FInputActionValue& Value);
	void DoJumpStart(const FInputActionValue& Value);
	void DoJumpEnd(const FInputActionValue& Value);
	
	UFUNCTION()
	void DeliverPackage();
	
	void HidePackage();
	void PullOutPackage();
	void EnterTruck();
	void ExitTruck();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* FirstPersonMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* FirstPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* BoxSocket;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ADeliveryPackage> BoxClass;
	UPROPERTY(VisibleAnywhere)
	ADeliveryPackage* DeliveryPackage;
	UPROPERTY(VisibleAnywhere)
	int32 DeliveredPackages = 0;
	
	TArray<AHouse*> Houses;
	ATruck* Truck;
	bool HasActiveOrder = false;
	bool IsHoldingPackage = false;
	bool IsInTruck = false;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* DefaultIMC;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* TruckIMC;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SpawnDebugBoxAction;
	
private:
	UEnhancedInputLocalPlayerSubsystem* GetSubsystem();
};
