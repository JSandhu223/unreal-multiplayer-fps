# Multiplayer FPS

Developed with Unreal Engine 5.8

## Game Architecture

### ShooterPlayerController

- Registers the input mapping context to the local player subsystem.
- Handles input actions related to basic character movement (i.e. looking around, moving, jumping, crouching).

### ShooterCharacter

- Has a `CombatComponent`.
- Handles input actions related to weapons (i.e. firing, aiming, reloading, switching), calling functions on the `CombatComponent`.
- Contains a first person arms mesh `Mesh1P` and full body mesh (default character mesh).
- Spawns the player inventory on call to `PossessedBy()`.
- Implements the `PlayerInterface`.
- Handles turn in place calculations in `Tick()`.

### CombatComponent

- Handles logic after the `ShooterCharacter` performs a weapon action.
  - Sphere trace for weapon fire.
  - RPC calls for firing weapon and aiming.
- Contains a `Weapon` array representing the player inventory.
- Contains an array of default weapons (set in the blueprint editor), all of which get spawned by the `ShooterCharacter` and get added to the inventory array.
- Holds a reference to the `CurrentWeapon`.
- Uses a boolean and a timer handle for handling full auto weapon fire.
- Handles multiplayer and replication logic.

### Weapon

- Contains a first person mesh `Mesh1P` and third person mesh `Mesh3P` for the weapon skeletal mesh.
- Uses enums EWeaponType (pistol, rifle, etc.) and EFireType (full auto, semi auto).
- Handles attachment of the respective skeletal mesh to the player's appropriate skeleton socket.
- Sets the visibility of each mesh based on the authority of the owning pawn.

### BP_Weapon

- The master blueprint for all weapons. Derives from the c++ `Weapon` class.
- Handles weapon effects such as firing effects, bullets impacts, decals, sounds, and animation montages.

### WeaponData

- A data asset that holds all data related to weapons.
    - `FPlayerAnims` - a custom struct that holds animation sequences, aim offsets, and blendspaces. Note that first person animations won't make use of aim offsets or blendspaces as these are strictly for the third person mesh.
    - `FMontageData` - a custom struct that holds animation montage assets for first person and third person.
    - `GripPoints` - maps the weapon type to a grip point socket.
    - `WeaponMontages` - maps the weapon type to a weapon animation montage not tied to that weapon type (these are animation montages that involve just the weapon mesh without a player skeleton).
    - `FirstPersonAnims` - maps the weapon type to a struct holding all first person animation assets for that particular weapon type.
    - `ThirdPersobAnims` - maps the weapon type to a struct holding all third person animation assets for that weapon type.
    - FirstPersonMontages - maps the weapon type to a struct holding first person animation montage assets for that weapon type (these animation montages involve a player skeleton).
    - ThirdPersonMontages - maps the weapon type to a struct holding third person animation montage assets for that weapon type (these animation montages involve a player skeleton).

### PlayerInterface

- Consists of getter functions, preventing any hard dependencies between the `ShoooterCharacter` and `Weapon`.
- The functions in this interface are marked as `BlueprintNativeEvents`. Becuase of this, we can call these functions as static functions, passing in the target object (i.e. the object we want to call the interface function on) as an argument.
  - For example, suppose we want to access the character's first person mesh from its `CombatComponent`. Since the function `GetMesh1P()` is a `BlueprintNativeEvent`, and we know that the `ShooterCharacter` implements the `PlayerInterface`, we can call `IPlayerInterface::Execute_GetMesh1P(GetOwner())`.

### ShooterGameplayTags

- Contains native gameplay tags, such as:
    - Weapon
        - Type
            - None
            - Rifle
            - Pistol

### ShooterTypes

- Contains an enum `ETurningInPlace` which helps with the turn in place logic for the third person animations.

## Multiplayer

- The `ShooterCharacter` is set to be replicated by default.
- The `Weapon` actor is replicated.
- The `CombatComponent` on the `ShooterCharacter` is registered for replication.
- In the `CombatComponent` class, the `Inventory` and `CurrentWeapon` fields are registered for replication.
- The `CombatComponent` has a `bAiming` boolean variable that is registered for replication. When it updates, a server RPC is called, which updates the value of this variable on the server, which in turn replicates it down to all clients.
- The `CombatComponent` handles first person weapon animations locally. Third person weapon animations are handled in the multicast RPC. The order of execution is as follows:
  - `Local_FireWeapon` -> `Server_FireWeapon` -> `Multicast_FireWeapon`

## Animation

- All first person animations are driven by `ABP_FirstPerson`.
- All third person animations are driven by `ABP_ThirdPerson`.
- Both animation blueprints use the `BlueprintAnimationInitialize` function/event to cache a reference to the `ShooterCharacter` c++ class by casting the result of `TryGetPawnOwner`.
- Both animation blueprints override `BlueprintThreadSafeUpdateAnimation` to access variables and functions from the cached `ShooterCharacter`.
    - Both animation blueprints retrieve the `CurrentWeapon` from the combat component of the cached `ShooterCharacter` to get its animations. The animations all live in the data asset and are mapped to by the weapon type.
    - Both animation blueprints read the `bAiming` variable from `ShooterCharacter`. This variable's result is then used to drive the hipfire and aiming animations on the equipped weapon.
    - Both animation blueprints have a *DefaultSlot* node to allow for playing of any animation montages, assuming the montage's slot group is set to *DefaultSlot*.
    - `ABP_ThirdPerson` retrieves the rotation of the `ShooterCharacter` and uses the pitch value to drive the aim offsets, allowing players to see each other looking up and down.
    - `ABP_ThirdPerson` uses FABRIK to ensure the weapon is oriented correctly to the character's hands. It does so by using the weapon's `FABRIK_Socket` and the character's `hand_r` bone. Note that this approach assumes that every weapon's skeletal mesh has a socket with the name "FABRIK_Socket".
    - `ABP_ThirdPerson` handles animation logic needed for turning in place. To do so, it needs access to variables from the `ShooterCharacter` such as
      - `TurningStatus`: Tells whether the character has turned past the left/right threshold.
      - `MovementOffsetYaw`: The delta between the character's movement rotation and aim rotation. This is used to drive the standing and crouching 1D blendspaces.
    - `ABP_ThirdPerson` uses the `NegatedAO_Yaw` from the `ShooterCharacter` as the *orientation angle* input to the **Orientation Warping** node. This allows the upper body to rotate while keeping the lower body in place.
