---
title: "Creating a WebAssembly Plugin System for Window Management (and beyond) in Miracle"
date: "2026-03-24"
tags: ["tech", "wayland", "miracle"]
---

Nothing quite makes you feel like you're using an exciting technology like a
plugin system. In fact, I would argue that this is one of the greatest draws of
the web today, at least from a business perspective.
Browsers provide a sandbox that runs JavaScript, HTML, CSS, and more. Developers
build apps using these technologies that the browser downloads and
executes safely. The entire program is delivered to the end-user on demand
within the confines of the browser.

Extensibility is a large part of what makes the modern web useful for businesses
and tinkerers alike. In my opinion, it is also a large part of what makes
software exciting in general, as it provides a point of entry for hobbyists to
craft a system to their liking.

## The State of Pluggable Window Management in Wayland

Ever since the transition from X11 to Wayland, the Linux desktop has lost the
ability to make window management pluggable. Different
window managers and desktops have explored ways of accomplishing this, each with pros and
cons. Hyprland has a [plugin system](https://hypr.land/plugins/) that
dynamically loads and executes `.so` files. GNOME has an [extension
system](https://extensions.gnome.org/)
where users write extensions in JavaScript. River has introduced a [new
Wayland protocol](https://isaacfreund.com/blog/river-window-management/) for
window management. Lots of ideas are floating around, but this is definitely
a place that is still being explored 🔭.

In my opinion, the ideal version of pluggable window management in Wayland would have
the following properties:

- **Lightweight runtime**: running plugins does not slow down the system
- **Secure**: plugins are only provided with the minimal amount of information that
  they need to do something useful
- **Synchronous**: async makes everything harder
- **Thread safe**: having to reason about thread safety makes everything hard
- **Language agnostic**: being tied to some weird scripting language makes everyone sad
- **Window manager agnostic**: it would be nice if the plugin worked across compositors

Without going into too much detail, each of the solutions mentioned previously
makes some tradeoff between these considerations. You can read more
about what I view as the pros and cons of each solution [below](#ps-survey-of-other-approaches).

## Miracle's WebAssembly Plugin System

Miracle's new plugin system is based on <u>WebAssembly</u>. WebAssembly is
a lightweight, bytecode language that can be embedded in multiple contexts.
While it began in the web, it is truly an agnostic runtime that has utility
everywhere from embedded devices, to browsers, and now desktops.

By using WebAssembly, we've created a plugin environment that very nearly meets
all of the criteria of my ideal plugin environment, minus the "window manager
agnostic" property. As I said previously, every solution will have some tradeoff 😅.

Here's how it all works. Miracle runs a WebAssembly bytecode engine inside of it
(we're using [WasmEdge](https://wasmedge.org/) specifically). At runtime, Miracle loads the
plugins (`.wasm` files) that were specified by the user. Miracle then looks for
special function signatures exposed by the WebAssembly bytecode and calls them
at the appropriate times. While running, the plugin may access information about Miracle
via thread-safe methods that Miracle explicitly exposes to the plugin. Additionally,
plugins can use [WASI](https://wasi.dev/) to access standard services from the
host. In this way, plugins run confined in the WebAssembly bytecode engine with
just the level of access that they require. And because WebAssembly is just
bytecode, the host can execute and run it with near-native speed.

Plugin authors can write plugins for Miracle in any language that
supports WebAssembly as a compilation target. The Miracle project will support an idiomatic
Rust crate for writing plugins for Miracle, which you can find
here: https://docs.miracle-wm.org/miracle_plugin/ 🦀. This crate abstracts away
the tricky communication-layer between Miracle and the WebAssembly plugin. If plugin
authors wish to write a plugin in another language, they can use the Rust implementation
as a reference.

The plugin interface has a pleasant design. Whenever an event happens on a
Miracle object, the plugin is prompted to take some action on an event. For example, here
is a handful of events that the plugin can handle:

- Placing, resizing, or transforming a window
- Focusing a window
- Requesting a workspace
- Handling a keyboard input event
- Handling a pointer input event
- Handling an animation update (window opening, window closing, window
  moving, workspace switching, etc.)
- and more!

If the plugin does not want to handle the event, they can simply not do so and let
Miracle (or the next plugin) handle it instead.

Plugins will also be notified when an event happens, such as:

- Window focused
- Window unfocused
- Window deleted
- Workspace created
- Workspace focused
- Workspace deleted
- and more!

The plugin need not respond to any of these events, but they could take some action if they choose
to. For example, a plugin may focus a window on the appropriate workspace when the workspace changes.

If you're interested in checking out an example usage of this API,
the best thus far is my [niri](https://github.com/niri-wm/niri) clone, aptly
named [miri](https://github.com/miracle-wm-org/miri-plugin). While this is in no
way a full Niri clone (yet!), it demonstrates the core semantics of the API, and
just how easy it is to use. Here is a little snippet of the window
management that goes on in Miri, with a bunch of details commented out:

```rust
// ...

impl Plugin for Miri {
    fn place_new_window(&mut self, info: &WindowInfo) -> Option<Placement> {
        if info.window_type != WindowType::Normal && info.window_type != WindowType::Freestyle {
            return None;
        }

        if info.state == WindowState::Attached {
            return None;
        }

        // ... commented out implementation details...

        Some(Placement::Freestyle(FreestylePlacement {
            top_left: Point::new(rect.x, rect.y),
            depth_layer: DepthLayer::Application,
            workspace: None,
            size: Size::new(rect.width, rect.height),
            transform: Mat4::IDENTITY,
            alpha: 1.0,
            movable: false,
            resizable: false,
        }))
    }

    fn window_deleted(&mut self, info: &WindowInfo) {
        // .. remove the window and scroll to a new window if its in focus ...
    }

    fn window_focused(&mut self, info: &WindowInfo) {
        // ... scroll the new window into focus ...
    }

    fn workspace_created(&mut self, workspace: &Workspace) {
        // ... set the workspace up with empty data ...
    }

    fn workspace_removed(&mut self, workspace: &Workspace) {
        // ... re-home windows on that workspace ...
    }

    fn workspace_focused(&mut self, _previous_id: Option<u64>, current: &Workspace) {
        // ... see if we should focus another window ...
    }

    fn workspace_area_changed(&mut self, workspace: &Workspace) {
        // ... change the size of existing windows ...
    }

    fn handle_keyboard_input(&mut self, event: KeyboardEvent) -> bool {
        // ... handle Niri-specific keyboard inputs ...
    }

    // ... and much more...!
}

```

Miri will continue to be developed over the coming months 😄.

But Miracle doesn't just offer window management via its plugin system.
One of the things that I always dislike about window managers is that I have to
configure them using some clunky file format. This format is good up to a point,
but it soon happens that my configuration is quite complex. What I _really_ want
in this situation is a <u>real</u> programming language. So that's just what I
added to Miracle! Instead of configuring your window manager in YAML, you can
now configure it in Rust 🦀. Here is a real example from my [dotfiles](https://github.com/mattkae/dotfiles/blob/master/config/miracle-wm/matts-config/src/lib.rs):

```rust
impl Plugin for MyPlugin {
    fn configure(&mut self) -> Option<Configuration> {
        let mut config: Configuration = Configuration::default();

        config.primary_modifier = Some(miracle_plugin::Modifier::Meta);

        let mut custom_actions: Vec<CustomKeyAction> = vec![];
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("d".to_string()),
            modifiers: vec![Modifier::Primary],
            command: "wofi --show=drun".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("S".to_string()),
            modifiers: vec![Modifier::Primary],
            command: "grimshot copy area".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("L".to_string()),
            modifiers: vec![Modifier::Primary],
            command: "swaylock".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("XF86AudioLowerVolume".to_string()),
            modifiers: vec![],
            command: "pamixer -d 10".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("XF86AudioRaiseVolume".to_string()),
            modifiers: vec![],
            command: "pamixer -i 10".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("XF86AudioMute".to_string()),
            modifiers: vec![],
            command: "pamixer -t".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("XF86MonBrightnessDown".to_string()),
            modifiers: vec![],
            command: "brightnessctl s 100-".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("XF86MonBrightnessUp".to_string()),
            modifiers: vec![],
            command: "brightnessctl s +100".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("E".to_string()),
            modifiers: vec![Modifier::Primary],
            command: "wlogout --protocol layer-shell".to_string(),
        });
        custom_actions.push(CustomKeyAction {
            action: miracle_plugin::BindingAction::Down,
            key: Key("M".to_string()),
            modifiers: vec![Modifier::Primary],
            command: "miraclemsg workspace music".to_string(),
        });

        config.custom_key_actions = Some(custom_actions);
        config.inner_gaps = Some(Gaps { x: 16, y: 16 });
        config.outer_gaps = Some(Gaps { x: 8, y: 8 });

        let mut startup_apps: Vec<StartupApp> = vec![];
        startup_apps.push(StartupApp {
            command: "waybar".to_string(),
            restart_on_death: false,
            no_startup_id: false,
            should_halt_compositor_on_death: false,
            in_systemd_scope: false,
        });
        startup_apps.push(StartupApp {
            command: "~/.local/bin/launch-swaybg.sh".to_string(),
            restart_on_death: false,
            no_startup_id: false,
            should_halt_compositor_on_death: false,
            in_systemd_scope: false,
        });
        startup_apps.push(StartupApp {
            command: "swaync".to_string(),
            restart_on_death: false,
            no_startup_id: false,
            should_halt_compositor_on_death: false,
            in_systemd_scope: false,
        });
        config.startup_apps = Some(startup_apps);

        config.terminal = Some("kitty".to_string());

        config.resize_jump = Some(50);
        config.border = Some(BorderConfig {
            size: 2,
            radius: 4.0,
            color: "0xbd93f9ff".to_string(),
            focus_color: "0x50fa7bff".to_string(),
        });

        Some(config)
    }
}
```

I love using a real programming language to configure. It makes me happy.

All this being said, I think that the WebAssembly solution provides a solid
middleground for all of my ideals:

- ✅ **Lightweight runtime**: WebAssembly is lightweight by design
- ✅ **Secure**: plugins are given only what they need from Miracle over the interface
- ✅ **Synchronous**: Miracle directly calls the plugin functions when it needs to
- ✅ **Thread safe**: Miracle guarantees thread-safe execution
- ✅ **Language agnostic**: WebAssembly is a compilation target for many languages (especially Rust)
- ⛔ **Window manager agnostic**: this is super-duper Miracle specific

I am excited about the work that is being done in this area right now, especially
River's implementation. Perhaps one day I will implement it myself in Miracle, but
I plan to focus mostly on Miracle's WASM plugin system in the near future.

## What's next?

The plugin system will be released in Miracle 0.9.0, which should be published later this week.
Miri will receive an official version afterward. I am excited to see what you all
build with the new API. I will be around to offer support for the plugins that you build
on the [matrix channel](https://matrix.to/#/#miracle-wm:matrix.org).

As you may be well aware, window management is only half the part of the desktop
story. The other part is the <u>shell</u>, which includes bars,
launchers, and a whole bunch of other stuff. My idea for this is to let Wayland clients
speak with plugins over the `MIRACLESOCK` (i.e. `SWAYSOCK`)
that Mir already exposes to clients like `waybar`. In this way, the window manager
will be able to tell clients about events so that the shell can update accordingly.
This bit is yet to be designed yet.

Miracle is turning into so much more than just the tiling window manager that I originally
designed it to be. The plugin system is a culmination of a lot of my thinking over the past
couple years, and I am very excited to get it into people's hands. Happy coding!

## Resources

- https://github.com/miracle-wm-org/miracle-wm - miracle-wm
- https://docs.miracle-wm.org/miracle_plugin/ - Miracle's plugin API
- https://github.com/miracle-wm-org/miri-plugin - Miri plugin for Miracle
- https://github.com/mattkae/dotfiles - My dotfiles with my custom plugin

## P.S.: Survey of Other Approaches

Below are my thoughts weighing the pros and cons of existing solutions.
Considering each of them guided much of my thinking as a designed Miracle's
system, so it is worthwhile noting them here. I still think that River's
solution is the best of the existing bunch, if only for its interest in being
cross-compositor.

### Dynamically Loading Shared Libraries

This approach has been taken by Hyprland.

**Pros**:

- Lightweight runtime\*: the plugin code runs the same as the native code.
- _Maximal control_: the plugin can do whatever is allowed on the system,
  including drawing complex graphics, reaching out to the internet, and more.
- _Common interface_: plugins are presumably programmed against the same
  interface as the rest of the program

**Cons**:

- _Security_: you must trust the plugin author as much as you trust the author
  of the host program
- _Language Lock-In_: plugins must typically use the language supported by the
  API, although it is feasible for other bindings to be written on top of this.

### Scripting Language

This approach is common to GNOME and KDE, and is usually accomplished with
JavaScript, Lua, or some other scripting language embedded in the desktop.

**Pros**:

- _Security_: theoretically, the JavaScript can be sandboxed such that it has
  minimal access ot the rest of the program.
- _Ease of use_: the general audience typically has a lot of exposure to
  JavaScript and othe scripting languages, making it easy

**Cons**:

- _Language Lock-In_: it is typically infeasible to author plugins in a language
  that is not provided by the host program, unless you're willing to do a LOT of work.
- _Heavy Runtime_: the host must ship an entire interpreter and runtime for the
  given scripting language inside of it.

### Wayland Protocol

This is the approach recently suggested by the River project, which I find very
interesting.

**Pros**:

- _Window Manager Agnostic_: any window manager can implement the protocol,
  making it a true global solution.
- _Language Agnostic_: Wayland clients can be written in any language, meaning
  that window manager can too!
- _Security_: the window manager client has the same permissions as any othe
  client running on the system, while remaining outside of the priveleged
  compositor, who knows about things like input events, PIDs, etc.
- _Medium control_: the compositor can access most of what it is interested in,
  minus the secret bits

**Cons**:

- _Async_: the Wayland protocol is asynchronous by nature, making frame-perfect
  management difficult (although the author of River is doing a great job here).

I actually think this is the best solution of the bunch thus far.
