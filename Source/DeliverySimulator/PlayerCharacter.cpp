#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SceneComponent.h"
#include "DeliveryPackage.h"
#include "House.h"
#include "Truck.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	BoxSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Box Socket"));
	BoxSocket->SetupAttachment(RootComponent);
	
	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> HouseActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHouse::StaticClass(), HouseActors);
	for (AActor* HouseActor : HouseActors)
	{
		AHouse* House = Cast<AHouse>(HouseActor);
		if (House)
		{
			Houses.Add(House);
		}
	}
	
	Truck = Cast<ATruck>(UGameplayStatics::GetActorOfClass(GetWorld(), ATruck::StaticClass()));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveInput);
		
		// Looking
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookInput);
		
		// Jumping
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::DoJumpStart);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoJumpEnd);
		
		// Debug Box
		EIC->BindAction(SpawnDebugBoxAction, ETriggerEvent::Started, this, &APlayerCharacter::SpawnDebugBoxInput);
		
		// Interacting
		EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::InteractInput);
	}
	
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (APlayerController* APC = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(APC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultIMC, 0);
		}
	}
}

void APlayerCharacter::MoveInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	AddMovementInput(GetActorRightVector(), MovementVector.X);
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
}

void APlayerCharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(-LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::InteractInput(const FInputActionValue& Value)
{
	if (Truck->PlayerOverlappingWithTrunk)
	{
		if (!Truck->HasPackageAtTrunk && IsHoldingPackage)
		{
			HidePackage();
			Truck->HasPackageAtTrunk = true;	
		}
		else if (Truck->HasPackageAtTrunk && !IsHoldingPackage)
		{
			PullOutPackage();
			Truck->HasPackageAtTrunk = false;
		}
	}
	if (Truck->PlayerOverlappingWithCabin)
	{
		if (!IsHoldingPackage && !IsInTruck)
		{
			EnterTruck();
		}
		else if (!IsHoldingPackage && IsInTruck)
		{
			ExitTruck();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("You cant enter Truck while holding package")));
		}
	}
}

void APlayerCharacter::SpawnDebugBoxInput(const FInputActionValue& Value)
{
	if (HasActiveOrder) return;
	if (Houses.Num() == 0) return;
	
	int32 RandomHouseIndex = FMath::RandRange(0, Houses.Num() - 1);
	if (Houses[RandomHouseIndex])
	{	
		Houses[RandomHouseIndex]->DeliveryPoint->SetVisibility(true);
		Houses[RandomHouseIndex]->OnPackageDeliveredDelegate.AddDynamic(this, &APlayerCharacter::DeliverPackage);
	}
	
	FTransform SpawnTransform = BoxSocket->GetComponentTransform();
	DeliveryPackage = GetWorld()->SpawnActor<ADeliveryPackage>(BoxClass, SpawnTransform);
	
	DeliveryPackage->AttachToComponent(BoxSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	if (DeliveryPackage)
	{
		IsHoldingPackage = true;
	}
	
	HasActiveOrder = true;
}

void APlayerCharacter::DoJumpStart(const FInputActionValue& Value)
{
	Jump();
}

void APlayerCharacter::DoJumpEnd(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacter::DeliverPackage()
{
	if (HasActiveOrder && IsHoldingPackage)
	{
		DeliveryPackage->Destroy();
		DeliveredPackages++;
		HasActiveOrder = false;
		IsHoldingPackage = false;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Package %d Delivered"), DeliveredPackages));
	}
}

void APlayerCharacter::HidePackage()
{
	DeliveryPackage->BoxMesh->SetVisibility(false);
	IsHoldingPackage = false;
}

void APlayerCharacter::PullOutPackage()
{
	DeliveryPackage->BoxMesh->SetVisibility(true);
	IsHoldingPackage = true;
}

void APlayerCharacter::EnterTruck()
{
	IsInTruck = true;
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetSubsystem())
	{
		Subsystem->RemoveMappingContext(DefaultIMC);
		Subsystem->AddMappingContext(TruckIMC, 0);
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Entering Truck")));
}

void APlayerCharacter::ExitTruck()
{
	IsInTruck = false;
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetSubsystem())
	{
		Subsystem->RemoveMappingContext(TruckIMC);
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Exiting Truck")));
}

UEnhancedInputLocalPlayerSubsystem* APlayerCharacter::GetSubsystem()
{
	if (APlayerController* APC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = APC->GetLocalPlayer())
		{
			return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		}
	}
	return nullptr;
}

