# lua system

This plugin for [The Machinery](https://ourmachinery.com/) makes it possible to add lua script to your entities.

## Compilation

You need `lua` and [`sol3`](https://github.com/ThePhD/sol2/). sol3 is header-only, so put it somewhere your compiler can find it or add its path to `includedirs` in `premake5.lua`.

## How to use

* Add a lua script to your assets.
* Add a `lua` component to an entity.
* Set the `script` field of the component to the script you added.

## Callbacks

Currently, the only available callback is `update` that gets called every frame.

## Entity API

All methods are called on an entity. In the `update` function, the entity on which the script is attached to is available via a variable named `entity`.
⚠️ This variable is only valid inside `update`. ⚠️

### transform component

* `set_position(x, y, z)`: Moves an entity.
* `set_rotation(x, y, z)`: Rotates an entity. All angles are in degrees.
