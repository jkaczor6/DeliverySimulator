#include "Truck.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

ATruck::ATruck()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	TruckMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Truck Mesh"));
	TruckMesh->SetupAttachment(Root);
	
	CabinPlayerDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("Cabin Player Detector"));
	CabinPlayerDetector->SetupAttachment(Root);
	
	TrunkPlayerDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("Trunk Player Detector"));
	TrunkPlayerDetector->SetupAttachment(Root);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(Root);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.0f;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	
	ExitPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Exit Point"));
	ExitPoint->SetupAttachment(Root);
	
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
}

void ATruck::BeginPlay()
{
	Super::BeginPlay();
	
	CabinPlayerDetector->OnComponentBeginOverlap.AddDynamic(this, &ATruck::OnCabinOverlapBegin);
	CabinPlayerDetector->OnComponentEndOverlap.AddDynamic(this, &ATruck::OnCabinOverlapEnd);
	
	TrunkPlayerDetector->OnComponentBeginOverlap.AddDynamic(this, &ATruck::OnTrunkOverlapBegin);
	TrunkPlayerDetector->OnComponentEndOverlap.AddDynamic(this, &ATruck::OnTrunkOverlapEnd);
	
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
}

void ATruck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetSpeed = CurrentThrottle * MaxSpeed;
	
	CurrentSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaTime, Acceleration / MaxSpeed);
	
	if (FMath::IsNearlyZero(CurrentThrottle))
	{
		CurrentSpeed = FMath::FInterpTo(CurrentSpeed, 0.0f, DeltaTime, Friction);
	}
	
	if (FMath::Abs(CurrentSpeed) < 5.0f)
	{
		CurrentSpeed = 0.0f;
	}
	
	FVector Move = GetActorForwardVector() * CurrentSpeed * DeltaTime;
	AddActorWorldOffset(Move, true);
	
	float SpeedFactor = FMath::Clamp(CurrentSpeed / MaxSpeed, 0.0f, 1.0f);
	float SteeringMultiplier = FMath::Lerp(0.7f, 0.7f, SpeedFactor);
	
	float Rotation = CurrentSteer * TurnSpeed * SteeringMultiplier * DeltaTime;
	AddActorLocalRotation(FRotator(0.0f, Rotation, 0.0f));
}

void ATruck::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(ExitAction, ETriggerEvent::Started, this, &ATruck::ExitInput);
		
		EIC->BindAction(CameraOrbitAction, ETriggerEvent::Triggered, this, &ATruck::CameraOrbitInput);
		
		EIC->BindAction(SteerAction, ETriggerEvent::Triggered, this, &ATruck::SteerInput);
		EIC->BindAction(SteerAction, ETriggerEvent::Completed, this, &ATruck::SteerReleased);
		
		EIC->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ATruck::ThrottleInput);
		EIC->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ATruck::ThrottleReleased);
	}
}

void ATruck::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (APlayerController* APC = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(APC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(TruckIMC, 0);
		}
	}
}

void ATruck::OnCabinOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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

void ATruck::OnTrunkOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		PlayerOverlappingWithTrunk = true;
	}
}

void ATruck::OnTrunkOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerOverlappingWithTrunk = false;
}

void ATruck::ExitInput()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	
	PC->Possess(PlayerCharacter);
	
	PlayerCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	PlayerCharacter->SetActorLocation(ExitPoint->GetComponentLocation());
	
	PlayerCharacter->SetActorHiddenInGame(false);
	PlayerCharacter->SetActorEnableCollision(true);
	
	PlayerCharacter->IsInTruck = false;
}

void ATruck::CameraOrbitInput(const FInputActionValue& Value)
{
	FVector2D CameraOrbitAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(-CameraOrbitAxisVector.X);
	AddControllerPitchInput(CameraOrbitAxisVector.Y);
}

void ATruck::SteerInput(const FInputActionValue& Value)
{
	CurrentSteer = Value.Get<float>();
}

void ATruck::SteerReleased(const FInputActionValue& Value)
{
	CurrentSteer = 0.0f;
}

void ATruck::ThrottleInput(const FInputActionValue& Value)
{
	CurrentThrottle = Value.Get<float>();
}

void ATruck::ThrottleReleased(const FInputActionValue& Value)
{
	CurrentThrottle = 0.0f;
}