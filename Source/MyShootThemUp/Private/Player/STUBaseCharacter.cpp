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
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponet"));
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->TargetArmLength = 300.0f;
    SpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponet"));
    CameraComponent->SetupAttachment(SpringArmComponent);

    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>(TEXT("HealthComponet"));

    HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthTextComponet"));
    HealthTextComponent->SetupAttachment(GetRootComponent());
    HealthTextComponent->SetOwnerNoSee(true);

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

    check(HealthComponent);
    check(HealthTextComponent);
    check(GetCharacterMovement());
    ensure(GetMesh());

    OnHealthChanged(HealthComponent->GetHealth(),0.0f);
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);
    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLand);
}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    const auto Health = HealthComponent->GetHealth();
}

// Called to bind functionality to input
void ASTUBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);
    check(WeaponComponent);
    if (InputType == EInputType::RawInput)
    {
        if (PlayerInputComponent)
        {
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
    IsMovingForward = Amount > 0.0f;
    if (Amount == 0.0f) return;
    AddMovementInput(GetActorForwardVector(), Amount);
    UE_LOG(LogTemp, Log, TEXT("MoveForward %f"), Amount);
}

void ASTUBaseCharacter::MoveRight(float Amount)
{
    if (Amount == 0.0f) return;
    AddMovementInput(GetActorRightVector(), Amount);
    UE_LOG(LogTemp, Log, TEXT("MoveRight %f"), Amount);
}

void ASTUBaseCharacter::Move(const FInputActionValue& Value)
{
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
        IsMovingForward = MovementVector.X > 0.0f;
        AddMovementInput(ForwardDirection, MovementVector.X);  // 前后移动
        AddMovementInput(RightDirection, MovementVector.Y);    // 左右移动[5,8](@ref)

        //UE_LOG(LogTemp, Log, TEXT("MoveForward %f"), MovementVector.X);
        //UE_LOG(LogTemp, Log, TEXT("MoveRight %f"), MovementVector.Y);
    }
}

void ASTUBaseCharacter::Look(const FInputActionValue& Value)
{
    LookVector = Value.Get<FVector2D>();
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        AddControllerPitchInput(LookVector.Y);
        AddControllerYawInput(LookVector.X);
    }
}

void ASTUBaseCharacter::Jumping(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsMovingOnGround())
    {
        ACharacter::Jump();
    }
}

void ASTUBaseCharacter::OnStartRunning()
{
    UE_LOG(LogTemp, Log, TEXT("OnStartRunning"));
    WantsToRun = true;
}

void ASTUBaseCharacter::OnStopRunning()
{
    WantsToRun = false;
}

void ASTUBaseCharacter::Run(const FInputActionValue& Value)
{
    WantsToRun = Value.Get<bool>();
}

bool ASTUBaseCharacter::IsRunning() const
{
    return WantsToRun && IsMovingForward && !GetVelocity().IsZero();
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero()) return 0.0f;
    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::OnDeath()
{
    UE_LOG(BaseCharacterLog, Display, TEXT("Player %s is dead"), *GetName());

    //PlayAnimMontage(DeathAnimMontage);

    GetCharacterMovement()->DisableMovement();

    SetLifeSpan(LifeSpanOnDeath);

    if (Controller)
    {
        Controller->ChangeState(NAME_Spectating);
    }
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponComponent->StopFire();

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

void ASTUBaseCharacter::OnHealthChanged(float Health,float HealthDelta)
{
    HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
}

void ASTUBaseCharacter::OnGroundLand(const FHitResult& Hit)
{
    /*    const auto FallVelocityZ = -GetCharacterMovement()->Velocity.Z; */
    const auto FallVelocityZ = -GetVelocity().Z;
    UE_LOG(BaseCharacterLog, Display, TEXT("On landed:velocity %f"), FallVelocityZ);
    if (FallVelocityZ < LandDamageVelocity.X) return;  // 跳跃时速度小于一定值，不受伤
    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandDamageVelocity, LandDamage, FallVelocityZ);
    UE_LOG(BaseCharacterLog, Display, TEXT("FinalDamage %f"), FinalDamage);
    TakeDamage(FinalDamage, FDamageEvent(), nullptr, nullptr);
}
