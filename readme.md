# lua system

This plugin for [The Machinery](https://ourmachinery.com/) makes it possible to add lua script to your entities.

## Compilation

You need `lua` and [`sol3`](https://github.com/ThePhD/sol2/). sol3 is header-only, so put it somewhere your compiler can find it or add its path to `includedirs` in `premake5.lua`.

## How to use

* Add a lua script to your assets.
* Add a `lua` component to an entity.
* Set the `script` field of the component to the script you added.

## Callbacks

Currently, the only available callback is `update` that gets called every frame. But initialisation code can be written outside of any function.

## Entity API

All methods are called on an entity. The entity on which the script is attached to is available via a variable named `entity`.

### transform component

* `set_position(x, y, z)`: Moves an entity.
* `set_rotation(x, y, z)`: Rotates an entity. All angles are in degrees.
* `set_scale(x, y, z)`: Scales an entity.
* `set_parent(parent_entity)`: Reparent an entity.
* `get_position()`: Retrieves the position of an entity.
* `get_rotation()`: Retrieves the position of an entity. All angles are in degrees.
* `get_scale()`: Retrieves the position of an entity.

All three `get_*` methods return 3 values (`x`, `y` and `z` in that order).

### light component

* `set_light_type(type)`: Changes the light's type: `light.point`, `light.spot`, `light.directional`, `light.ibl`, `light.sphere`, `light.rectangle` or `light.disk`.
* `set_light_unit_type(type)`: Changes the light's unit: `light.candela`, `light.lumen`, `light.lux`, `light.nits` or `light.ev100`.
* `set_light_color_type(type)`: Changes the light's color type: `light.rgb` or `light.kelvin`.
* `set_light_color(r, g, b)`: Changes the light's color.
* `set_light_type()`: Retrieves the light's type.
* `set_light_unit_type()`: Retrieves the light's unit.
* `set_light_color_type()`: Retrieves the light's color type.
* `get_light_color()`: Retrieves the light's color. Returns 3 values (`r`, `g` and `b` in that order).

⚠️ Not all units are supported by all types, see the [documentation](https://ourmachinery.com//apidoc/plugins/default_render_pipe/light_component.h.html#enumtm_light_component_unit_type). 

## Properties

You can declare a global variable named `properties` being a table where the key is the property's name and the value is the default value of the property. [Example](examples/properties.lua)  
They will appear in the property tab of the lua component.

⚠️ It doesn't do anything. They are displayed but not editable nor used/usable. ⚠️
