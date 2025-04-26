// Shoot Them Up Game,All Right Reserved.

#include "Player/STUBaseCharacter.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All);

// Sets default values
ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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

    // 创建一个HealthComponent
    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>(TEXT("HealthComponet"));

    // 创建一个HealthTextComponent，并将其附加到角色的根组件上
    HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthTextComponet"));
    HealthTextComponent->SetupAttachment(GetRootComponent());
    // 设置HealthTextComponent的可见性
    HealthTextComponent->SetOwnerNoSee(true);

    // 创建一个WeaponComponent
    WeaponComponent = CreateDefaultSubobject<USTUWeaponComponent>(TEXT("WeaponComponet"));

    // 设置输入组件类型为EnhancedInputComponent
    InputComponentClass = UEnhancedInputComponent::StaticClass();
    // 自动加载输入资产（可选）
    static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveRef(TEXT("/Game/Player/Input/IA_Move"));
    MoveAction = IA_MoveRef.Object;
    static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookRef(TEXT("/Game/Player/Input/IA_Look"));
    LookAction = IA_LookRef.Object;
}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
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

    // 检查HealthComponent、HealthTextComponent、GetCharacterMovement()和GetMesh()是否为空
    check(HealthComponent);
    check(HealthTextComponent);
    check(GetCharacterMovement());
    ensure(GetMesh());

    // 调用OnHealthChanged函数，更新HealthTextComponent的文本
    OnHealthChanged(HealthComponent->GetHealth(),0.0f);
    // 绑定HealthComponent的OnDeath和OnHealthChanged事件到相应的函数
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);
    // 绑定LandedDelegate到OnGroundLand函数
    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLand);
}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 获取HealthComponent的Health值
    const auto Health = HealthComponent->GetHealth();
}

// Called to bind functionality to input
void ASTUBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // 检查PlayerInputComponent和WeaponComponent是否为空
    check(PlayerInputComponent);
    check(WeaponComponent);
    if (InputType == EInputType::RawInput)
    {
        if (PlayerInputComponent)
        {
            // 绑定输入轴到相应的函数
            PlayerInputComponent->BindAxis("MoveForward", this, &ASTUBaseCharacter::MoveForward);
            PlayerInputComponent->BindAxis("MoveRight", this, &ASTUBaseCharacter::MoveRight);
            PlayerInputComponent->BindAxis("LookUp", this, &ASTUBaseCharacter::AddControllerPitchInput);
            PlayerInputComponent->BindAxis("TurnAround", this, &ASTUBaseCharacter::AddControllerYawInput);
            PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUBaseCharacter::Jump);
            PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUBaseCharacter::OnStartRunning);
            PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUBaseCharacter::OnStopRunning);
            PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent.Get(), &USTUWeaponComponent::StartFire);
            PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent.Get(), &USTUWeaponComponent::StopFire);
            PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent.Get(), &USTUWeaponComponent::NextWeapon);
            PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent.Get(), &USTUWeaponComponent::Reload);
        }
    }
    else if (InputType == EInputType::EnhancedInput)
    {
        // 将输入组件转换为EnhancedInputComponent
        UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
        if (EnhancedInput)
        {
            // 绑定MoveAction到回调函数
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASTUBaseCharacter::Move);
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASTUBaseCharacter::Look);
            EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ASTUBaseCharacter::Jumping);
            EnhancedInput->BindAction(RunAction, ETriggerEvent::Started, this, &ASTUBaseCharacter::OnStartRunning);
            EnhancedInput->BindAction(RunAction, ETriggerEvent::Completed, this, &ASTUBaseCharacter::OnStopRunning);
            EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, WeaponComponent.Get(), &USTUWeaponComponent::StartFire);
            EnhancedInput->BindAction(FireAction, ETriggerEvent::Completed, WeaponComponent.Get(), &USTUWeaponComponent::StopFire);
            EnhancedInput->BindAction(WeaponAction, ETriggerEvent::Started, WeaponComponent.Get(), &USTUWeaponComponent::NextWeapon);
            EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, WeaponComponent.Get(), &USTUWeaponComponent::Reload);
        }
    }
}

void ASTUBaseCharacter::MoveForward(float Amount)
{
    // 设置IsMovingForward的值
    IsMovingForward = Amount > 0.0f;
    if (Amount == 0.0f) return;
    // 添加移动输入
    AddMovementInput(GetActorForwardVector(), Amount);
    UE_LOG(LogTemp, Log, TEXT("MoveForward %f"), Amount);
}

void ASTUBaseCharacter::MoveRight(float Amount)
{
    if (Amount == 0.0f) return;
    // 添加移动输入
    AddMovementInput(GetActorRightVector(), Amount);
    UE_LOG(LogTemp, Log, TEXT("MoveRight %f"), Amount);
}

void ASTUBaseCharacter::Move(const FInputActionValue& Value)
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

        //UE_LOG(LogTemp, Log, TEXT("MoveForward %f"), MovementVector.X);
        //UE_LOG(LogTemp, Log, TEXT("MoveRight %f"), MovementVector.Y);
    }
}

void ASTUBaseCharacter::Look(const FInputActionValue& Value)
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

void ASTUBaseCharacter::Jumping(const FInputActionValue& Value)
{
    // 如果角色在地面上，则调用Jump函数
    if (GetCharacterMovement()->IsMovingOnGround())
    {
        ACharacter::Jump();
    }
}

void ASTUBaseCharacter::OnStartRunning()
{
    UE_LOG(LogTemp, Log, TEXT("OnStartRunning"));
    // 设置WantsToRun的值
    WantsToRun = true;
}

void ASTUBaseCharacter::OnStopRunning()
{
    // 设置WantsToRun的值
    WantsToRun = false;
}

void ASTUBaseCharacter::Run(const FInputActionValue& Value)
{
    // 设置WantsToRun的值
    WantsToRun = Value.Get<bool>();
}

bool ASTUBaseCharacter::IsRunning() const
{
    // 如果WantsToRun为true，IsMovingForward为true，且角色的速度不为零，则返回true
    return WantsToRun && IsMovingForward && !GetVelocity().IsZero();
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    // 如果角色的速度为零，则返回0.0f
    if (GetVelocity().IsZero()) return 0.0f;
    // 获取角色的速度方向
    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    // 计算角色速度方向与角色前向方向的夹角
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    // 计算角色速度方向与角色前向方向的叉积
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
    // 将夹角转换为度数
    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
    // 如果叉积为零，则返回夹角，否则返回夹角乘以叉积的符号
    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::OnDeath()
{
    UE_LOG(BaseCharacterLog, Display, TEXT("Player %s is dead"), *GetName());

    //PlayAnimMontage(DeathAnimMontage);

    // 禁用角色的移动
    GetCharacterMovement()->DisableMovement();

    // 设置角色的生命周期
    SetLifeSpan(LifeSpanOnDeath);

    // 如果角色有控制器，则将其状态改为观察者状态
    if (Controller)
    {
        Controller->ChangeState(NAME_Spectating);
    }
    // 设置角色的CapsuleComponent的碰撞响应
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    // 停止角色的射击
    WeaponComponent->StopFire();

    // 设置角色的Mesh的碰撞模式和物理模拟
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

void ASTUBaseCharacter::OnHealthChanged(float Health,float HealthDelta)
{
    // 更新HealthTextComponent的文本
    HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
}

void ASTUBaseCharacter::OnGroundLand(const FHitResult& Hit)
{
    /*    const auto FallVelocityZ = -GetCharacterMovement()->Velocity.Z; */
    // 获取角色的速度在Z轴上的分量
    const auto FallVelocityZ = -GetVelocity().Z;
    UE_LOG(BaseCharacterLog, Display, TEXT("On landed:velocity %f"), FallVelocityZ);
    // 如果角色的速度小于一定值，则不受伤
    if (FallVelocityZ < LandDamageVelocity.X) return;  // 跳跃时速度小于一定值，不受伤
    // 计算最终伤害
    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandDamageVelocity, LandDamage, FallVelocityZ);
    UE_LOG(BaseCharacterLog, Display, TEXT("FinalDamage %f"), FinalDamage);
    // 伤害角色
    TakeDamage(FinalDamage, FDamageEvent(), nullptr, nullptr);
}