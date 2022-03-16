// Fill out your copyright notice in the Description page of Project Settings.


#include "Players/PlayerCharacter.h"

#include <ThirdParty/Imath/Deploy/Imath-3.1.3/include/Imath/ImathFun.h>

#include "VectorTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Init Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Init Input stuff
	GamepadTurnRate = 50.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Init movement stuff. All of this can be adjusted in blueprints
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f); // ...at this rotation rate

	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraDistance = BaseCameraDistance;
	GetCharacterMovement() -> bOrientRotationToMovement = true;
}

// Called every frame
void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraBoom(DeltaTime);
}

// Input

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	// Using the default character jump for the jump.
	PlayerInputComponent -> BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent -> BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Movement Input
	PlayerInputComponent -> BindAxis("Forward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent -> BindAxis("Right", this, &APlayerCharacter::MoveRight);

	// Look Input
	PlayerInputComponent -> BindAxis("LookUp_Gamepad", this, &APlayerCharacter::LookUpGamepad);
	PlayerInputComponent -> BindAxis("LookRight_Gamepad", this, &APlayerCharacter::LookRightGamepad);
	
	PlayerInputComponent -> BindAxis("LookUp_Mouse", this, &APawn::AddControllerPitchInput); // Mouse can use the regular controller stuff
	PlayerInputComponent -> BindAxis("LookRight_Mouse", this, &APawn::AddControllerYawInput); // Mouse can use the regular controller stuff

	// Zoom
	PlayerInputComponent -> BindAction("Zoom", IE_Pressed, this, &APlayerCharacter::EnterZoom);
	PlayerInputComponent -> BindAction("Zoom", IE_Released, this, &APlayerCharacter::ExitZoom);
}

void APlayerCharacter::MoveForward(const float Value)
{
	if(Controller != nullptr && Value != 0)
	{
		// Figure out which way is forward
		const FRotator Rotation = Controller -> GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get the forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(const float Value)
{
	if(Controller != nullptr && Value != 0)
	{
		const FRotator Rotation = Controller -> GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::LookUpGamepad(const float Value)
{
}

void APlayerCharacter::LookRightGamepad(const float Value)
{
}

void APlayerCharacter::EnterZoom()
{
	ZoomTimer = 0;
	CameraState = ZoomIn;
	GetCharacterMovement() -> bOrientRotationToMovement = false;
}

void APlayerCharacter::ExitZoom()
{
	ZoomTimer = 0;
	CameraState = ZoomOut;
	GetCharacterMovement() -> bOrientRotationToMovement = true;
}

void APlayerCharacter::UpdateCameraBoom(float DeltaTime)
{
	ZoomTimer += DeltaTime;
	float NormalizedTime = ZoomTimer / CameraZoomTime;
	NormalizedTime = FMath::Clamp(NormalizedTime, 0, 1);
	
	switch (CameraState)
	{
		case ZoomIn :
			CameraDistance = Imath_3_1::lerp(BaseCameraDistance, ZoomCameraDistance, NormalizedTime);
			if(NormalizedTime >= 1)
			{
				ZoomTimer = 0;
				CameraState = ZoomedIn;
			}
			break;
		case ZoomOut :
			CameraDistance = Imath_3_1::lerp(ZoomCameraDistance, BaseCameraDistance, NormalizedTime);
			if(NormalizedTime >= 1)
			{
				ZoomTimer = 0;
				CameraState = ZoomedOut;
			}
			break;
		default: ;
	}

	

	FVector CameraPos;
	FRotator CameraRot;

	GetActorEyesViewPoint(CameraPos, CameraRot);

	const FVector Start = CameraPos;
	FVector End = Start + CameraRot.Vector() * CameraTraceDistance;

	FHitResult OutHit;

	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.5, 0, 5);
	
	CameraBoom -> TargetArmLength = CameraDistance;
}