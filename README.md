# Multiplayer FPS

Developed with Unreal Engine 5.8

## Game Architecture

### ShooterPlayerController

- Registers the input mapping context to the local player subsystem
- Handles input actions related to basic character movement (i.e. looking around, moving, jumping, crouching)

### ShooterCharacter

- Has a `CombatComponent`
- Handles input actions related to weapons (i.e. firing, aiming, reloading, switching), calling functions on the `CombatComponent`
- Contains a first person arms mesh `Mesh1P` and full body mesh `Mesh3P`
- Spawns the player inventory on call to `PossessedBy()`
- Implements the `PlayerInterface`

### CombatComponent

- Responsible for what happens after the `ShooterCharacter` performs a weapon action
- Contains a `Weapon` array representing the player inventory
- Contains an array of default weapons (set in the blueprint editor), all of which get spawned by the `ShooterCharacter` and get added to the inventory array
- Holds a reference to the `CurrentWeapon`
- Handles multiplayer and replication logic

### Weapon

- Contains a first person mesh `Mesh1P` and third person mesh `Mesh3P` for the weapon skeletal mesh
- Handles attachment of the respective skeletal mesh to the player's appropriate skeleton socket
- Sets the visibility of each mesh based on the authority of the owming pawn

### WeaponData

- A data asset that holds all data related to weapons
- Contains the following data:

    - A `TMap` field that maps a WeaponType gameplay tag to an FName
    - Two `TMap` fields that maps a WeaponType to an FPlayerAnims struct, which holds animation data such as animation sequences and blendspaces. The first TMap corresponds to first person animations and the second TMap corresponds to third person animations.

### PlayerInterface

- Consists of getter functions, preventing any hard dependencies between the `ShoooterCharacter` and `Weapon`

### ShooterGameplayTags

- Contains native gameplay tags, such as:
    - Weapon
        - Type
            - None
            - Rifle
            - Pistol

## Multiplayer

### Replication

- The `Weapon` actor is replicated
- The `CombatComponent` on the `ShooterCharacter` is registered for replication
- In the `CombatComponent` class, the `Inventory` and `CurrentWeapon` are registered for replication
