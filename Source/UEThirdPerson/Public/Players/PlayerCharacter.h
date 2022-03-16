// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class UETHIRDPERSON_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
    
    	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    	class USpringArmComponent* CameraBoom;
    	
    	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    	class UCameraComponent* FollowCamera;
    
    	// Variables
    	public:
    		// Sets default values for this character's properties
    		APlayerCharacter();
    
    		//Input variables
    		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
    		float GamepadTurnRate;
    
    		// Camera Variables
    		UPROPERTY(EditAnywhere, BlueprintReadWrite,Category=Camera)
    		float BaseCameraDistance = 300.0f;
    
    		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
    		float ZoomCameraDistance = 175.0f;
    
    		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
    		float CameraZoomTime = 0.25f;
    
    		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
    		float CameraTraceDistance = 200.0f;
    
    	private:
    		float CameraDistance = 0;
    
    		enum BoomStates
    		{
    			ZoomedIn,
    			ZoomedOut,
    			ZoomIn,
    			ZoomOut,
    		};
    
    		BoomStates CameraState = ZoomedOut;
    
    		float ZoomTimer = 0.0f;
    
    	// Functions
    	public:	
    		// Called every frame
    		virtual void Tick(float DeltaTime) override;
    
    		//Input
    		
    		// Called to bind functionality to input
    		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    		
    		/** Returns CameraBoom subobject **/
    		FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    		/** Returns FollowCamera subobject **/
    		FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
    	
    	protected:
    		// Called when the game starts or when spawned
    		virtual void BeginPlay() override;
    
    		// Input
    
    		// Movement Functions
    		virtual void MoveForward(float Value);
    		virtual void MoveRight(float Value);
    
    		// Look Functions
    		virtual void LookUpGamepad(float Value);
    		virtual void LookRightGamepad(float Value);
    
    		// Look Zoom
    		virtual void EnterZoom();
    		virtual void ExitZoom();
    
    	private:
    		virtual void UpdateCameraBoom(float DeltaTime);

};
