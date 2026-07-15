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
- Handles turn in place logic in `Tick()`.

### CombatComponent

- Responsible for what happens after the `ShooterCharacter` performs a weapon action.
- Contains a `Weapon` array representing the player inventory.
- Contains an array of default weapons (set in the blueprint editor), all of which get spawned by the `ShooterCharacter` and get added to the inventory array.
- Holds a reference to the `CurrentWeapon`.
- Handles multiplayer and replication logic.

### Weapon

- Contains a first person mesh `Mesh1P` and third person mesh `Mesh3P` for the weapon skeletal mesh.
- Handles attachment of the respective skeletal mesh to the player's appropriate skeleton socket.
- Sets the visibility of each mesh based on the authority of the owming pawn.

### WeaponData

- A data asset that holds all data related to weapons.
- Contains the following data:
    - A `TMap` field that maps a WeaponType gameplay tag to an FName.
    - Two `TMap` fields that maps a WeaponType to an FPlayerAnims struct, which holds animation data such as animation sequences and blendspaces. The first TMap corresponds to first person animations and the second TMap corresponds to third person animations.

### PlayerInterface

- Consists of getter functions, preventing any hard dependencies between the `ShoooterCharacter` and `Weapon`.

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

### Replication

- The `ShooterCharacter` is set to be replicated by default.
- The `Weapon` actor is replicated.
- The `CombatComponent` on the `ShooterCharacter` is registered for replication.
- In the `CombatComponent` class, the `Inventory` and `CurrentWeapon` fields are registered for replication.
- The `CombatComponent` has a `bAiming` boolean variable that is registered for replication. When it updates, a server RPC is called, which updates the value of this variable on the server, which in turn replicates it down to all clients.

## Animation

- All first person animations are driven by `ABP_FirstPerson`.
- All third person animations are driven by `ABP_ThirdPerson`.
- Both animation blueprints use the `BlueprintAnimationInitialize` function/event to cache a reference to the `ShooterCharacter` c++ class by casting the result of `TryGetPawnOwner`.
- Both animation blueprints override `BlueprintThreadSafeUpdateAnimation` to access variables and functions from the cached `ShooterCharacter`.
    - Both animation blueprints retrieve the `CurrentWeapon` from the combat component of the cached `ShooterCharacter` to get its animations. The animations all live in the data asset and are tied to the weapon type.
    - Both animation blueprints read the `bAiming` variable from `ShooterCharacter`. This variable's result is then used to drive the hipfire and aiming animations on the equipped weapon.
    - `ABP_ThirdPerson` retrieves the rotation of the `ShooterCharacter` and uses the pitch value to drive the aim offsets, allowing players to see each other looking up and down.
    - `ABP_ThirdPerson` uses FABRIK to ensure the weapon is oriented correctly to the character's hands. It does so by using the weapon's `FABRIK_Socket` and the character's `hand_r` bone. Note that this approach assumes that every weapon's skeletal mesh has a socket with the name "FABRIK_Socket".
    - `ABP_ThirdPerson` handles animation logic needed for turning in place. To do so, it needs access to variables from the `ShooterCharacter` such as
      - `TurningStatus`: Tells whether the character has turned past the left/right threshold.
      - `MovementOffsetYaw`: The delta between the character's movement rotation and aim rotation. This is used to drive the standing and crouching 1D blendspaces.
    - `ABP_ThirdPerson` uses the `AO_Yaw` from the `ShooterCharacter` as the *orientation angle* for **Orientation Warping**. This allows the upper body to rotate while keeping the lower body in place.
