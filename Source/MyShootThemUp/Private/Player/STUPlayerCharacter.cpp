// Shoot Them Up Game,All Right Reserved.

#include "Player/STUPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/STUWeaponComponent.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

ASTUPlayerCharacter::ASTUPlayerCharacter(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // 创建一个SpringArmComponent，并将其附加到角色的根组件上
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponet"));
    SpringArmComponent->SetupAttachment(GetRootComponent());
    // 设置SpringArmComponent的旋转控制
    SpringArmComponent->bUsePawnControlRotation = true;
    // 设置SpringArmComponent的长度
    SpringArmComponent->TargetArmLength = 300.0f;
    // 设置SpringArmComponent的SocketOffset
    SpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);

    // 创建一个CameraComponent，并将其附加到SpringArmComponent上
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponet"));
    CameraComponent->SetupAttachment(SpringArmComponent);

    CameraCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CameraCollisionComponent"));
    CameraCollisionComponent->SetupAttachment(CameraComponent);
    CameraCollisionComponent->SetSphereRadius(10.f);
    CameraCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

    // 设置输入组件类型为EnhancedInputComponent
    InputComponentClass = UEnhancedInputComponent::StaticClass();
    // 自动加载输入资产（可选）
    static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveRef(TEXT("/Game/Player/Input/IA_Move"));
    MoveAction = IA_MoveRef.Object;
    static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookRef(TEXT("/Game/Player/Input/IA_Look"));
    LookAction = IA_LookRef.Object;
}

void ASTUPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    // 获取玩家控制器
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    // 与上面的同样，但不同写法。
    // APlayerController* PlayerController = GetController<APlayerController>();
    if (PlayerController)
    {
        // 注册输入映射
        UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        Subsystem->AddMappingContext(DefaultMapping, 1);  // 优先级0[1,8](@ref)
    }

    check(CameraCollisionComponent);
    CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionBeginOverlap);
    CameraCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionEndOverlap);
}

void ASTUPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // 检查PlayerInputComponent和WeaponComponent是否为空
    check(PlayerInputComponent);
    check(WeaponComponent);
    if (InputType == EInputType::RawInput)
    {
        SetInputType(EInputType::RawInput);
        if (PlayerInputComponent)
        {
            // 绑定输入轴到相应的函数
            PlayerInputComponent->BindAxis("MoveForward", this, &ASTUPlayerCharacter::MoveForward);
            PlayerInputComponent->BindAxis("MoveRight", this, &ASTUPlayerCharacter::MoveRight);
            PlayerInputComponent->BindAxis("LookUp", this, &ASTUPlayerCharacter::AddControllerPitchInput);
            PlayerInputComponent->BindAxis("TurnAround", this, &ASTUPlayerCharacter::AddControllerYawInput);
            PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUPlayerCharacter::Jump);
            PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUPlayerCharacter::OnStartRunning);
            PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUPlayerCharacter::OnStopRunning);
            PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent.Get(), &USTUWeaponComponent::StartFire);
            PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent.Get(), &USTUWeaponComponent::StopFire);
            PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent.Get(), &USTUWeaponComponent::NextWeapon);
            PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent.Get(), &USTUWeaponComponent::Reload);
        }
    }
    else if (InputType == EInputType::EnhancedInput)
    {
        SetInputType(EInputType::EnhancedInput);
        // 将输入组件转换为EnhancedInputComponent
        UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
        if (EnhancedInput)
        {
            // 绑定MoveAction到回调函数
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASTUPlayerCharacter::Move);
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASTUPlayerCharacter::Look);
            EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ASTUPlayerCharacter::Jumping);
            EnhancedInput->BindAction(RunAction, ETriggerEvent::Started, this, &ASTUPlayerCharacter::OnStartRunning);
            EnhancedInput->BindAction(RunAction, ETriggerEvent::Completed, this, &ASTUPlayerCharacter::OnStopRunning);
            EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, WeaponComponent.Get(), &USTUWeaponComponent::StartFire);
            EnhancedInput->BindAction(FireAction, ETriggerEvent::Completed, WeaponComponent.Get(), &USTUWeaponComponent::StopFire);
            EnhancedInput->BindAction(WeaponAction, ETriggerEvent::Started, WeaponComponent.Get(), &USTUWeaponComponent::NextWeapon);
            EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, WeaponComponent.Get(), &USTUWeaponComponent::Reload);
        }
    }
}

void ASTUPlayerCharacter::MoveForward(float Amount)
{
    // 设置IsMovingForward的值
    IsMovingForward = Amount > 0.0f;
    if (Amount == 0.0f) return;
    // 添加移动输入
    AddMovementInput(GetActorForwardVector(), Amount);
    UE_LOG(LogTemp, Log, TEXT("MoveForward %f"), Amount);
}

void ASTUPlayerCharacter::MoveRight(float Amount)
{
    if (Amount == 0.0f) return;
    // 添加移动输入
    AddMovementInput(GetActorRightVector(), Amount);
    UE_LOG(LogTemp, Log, TEXT("MoveRight %f"), Amount);
}

void ASTUPlayerCharacter::Move(const FInputActionValue& Value)
{
    // 获取输入值
    MovementVector = Value.Get<FVector2D>();

    // 将输入方向转换为角色移动方向（考虑摄像机旋转）
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        const FRotator YawRotation(0, PlayerController->GetControlRotation().Yaw, 0);
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        //// 直接通过移动组件施加输入
        // if (MovementComponent)
        //{
        //	MovementComponent->AddInputVector(ForwardDirection * MovementVector.Y); // W/S 控制前后
        //	MovementComponent->AddInputVector(RightDirection * MovementVector.X);
        // }
        // 设置IsMovingForward的值
        IsMovingForward = MovementVector.X > 0.0f;
        // 添加移动输入
        AddMovementInput(ForwardDirection, MovementVector.X);  // 前后移动
        AddMovementInput(RightDirection, MovementVector.Y);    // 左右移动[5,8](@ref)

        // UE_LOG(LogTemp, Log, TEXT("MoveForward %f"), MovementVector.X);
        // UE_LOG(LogTemp, Log, TEXT("MoveRight %f"), MovementVector.Y);
    }
}

void ASTUPlayerCharacter::Look(const FInputActionValue& Value)
{
    // 获取输入值
    LookVector = Value.Get<FVector2D>();
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        // 添加旋转输入
        AddControllerPitchInput(LookVector.Y);
        AddControllerYawInput(LookVector.X);
    }
}

void ASTUPlayerCharacter::Jumping(const FInputActionValue& Value)
{
    // 如果角色在地面上，则调用Jump函数
    if (GetCharacterMovement()->IsMovingOnGround())
    {
        ACharacter::Jump();
    }
}

void ASTUPlayerCharacter::OnStartRunning()
{
    UE_LOG(LogTemp, Log, TEXT("OnStartRunning"));
    // 设置WantsToRun的值
    WantsToRun = true;
}

void ASTUPlayerCharacter::OnStopRunning()
{
    // 设置WantsToRun的值
    WantsToRun = false;
}

void ASTUPlayerCharacter::Run(const FInputActionValue& Value)
{
    // 设置WantsToRun的值
    WantsToRun = Value.Get<bool>();
}

bool ASTUPlayerCharacter::IsRunning() const
{
    // 如果WantsToRun为true，IsMovingForward为true，且角色的速度不为零，则返回true
    return WantsToRun && IsMovingForward && !GetVelocity().IsZero();
}

void ASTUPlayerCharacter::OnDeath()
{
    Super::OnDeath();
    // 如果角色有控制器，则将其状态改为观察者状态
    if (Controller)
    {
        Controller->ChangeState(NAME_Spectating);
    }
}

void ASTUPlayerCharacter::OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    CheckCamereOverlap();
}

void ASTUPlayerCharacter::OnCameraCollisionEndOverlap(
    UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    CheckCamereOverlap();
}

void ASTUPlayerCharacter::CheckCamereOverlap()
{
    const auto HideMesh = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
    GetMesh()->SetOwnerNoSee(HideMesh);

    TArray<USceneComponent*> MeshChildren;
    GetMesh()->GetChildrenComponents(true, MeshChildren);

    for (auto MeshChild : MeshChildren)
    {
        if (const auto MeshChildGeometry = Cast<UPrimitiveComponent>(MeshChild))
        {
            MeshChildGeometry->SetOwnerNoSee(HideMesh);
        }
    }
}

void ASTUPlayerCharacter::SetInputType(EInputType NewInputType) 
{
    OnInputTypeChanged.Broadcast(NewInputType);
}
