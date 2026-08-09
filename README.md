# Road Rumble

[![CI](https://github.com/IbrahimAwad98/Road-Rumble/actions/workflows/ci.yml/badge.svg)](https://github.com/IbrahimAwad98/Road-Rumble/actions/workflows/ci.yml)

CI compiles both binaries but does not run them.

A four-player top-down racing game written in C with SDL2, where a dedicated
server relays car state between clients over UDP.

Built as a team project at KTH. All four players share one fixed screen — there
is no split-screen and no scrolling camera; the whole track fits in the window.

## What works

**Racing**

- Four cars on a tile-based track, three laps, first to finish wins.
- Car-to-car collision (positional push-apart with speed damping) and
  car-to-world collision against barrels, crates, and off-track tiles.
- Lap counting on finish-line crossing, with a `laps/3` HUD and a winner screen
  that resets the round.
- Drift mechanic with tyre trails that persist behind the car.
- Boost: drive over the boost tile on lap 2 to unlock a single 5-second burst
  at double acceleration, triggered with `Right Shift`, drawn as an animated
  flame. Both drift and boost are replicated to the other players.
- Animated traffic-light countdown with three sound cues, started by the
  server once every player has joined.

**Networking**

- Dedicated server binary, separate from the client, on UDP port **55000**
  via SDL_net.
- Four player slots, address-based registration, "server full" rejection, and
  a five-second inactivity timeout.
- Round-trip ping measured once per second and drawn in the corner,
  colour-coded by latency.

**Interface**

- Menus for Start, Multiplayer, and Options, in two complete themes: classic
  and dark.
- Options: theme toggle, WASD/arrow-key remapping, five-step music and SFX
  sliders, mute.
- Multiplayer menu with text-entry fields for the server IP and player ID.
- Diagnostic mode (`--test`) that checks the renderer, the font, and SDL_net
  initialisation before starting.

## Requirements

- A C compiler (GCC) and `make`
- SDL2 plus `SDL2_image`, `SDL2_ttf`, `SDL2_net`, `SDL2_mixer`

```bash
# Debian / Ubuntu
sudo apt install build-essential libsdl2-dev libsdl2-image-dev \
                 libsdl2-ttf-dev libsdl2-net-dev libsdl2-mixer-dev

# macOS
brew install sdl2 sdl2_image sdl2_ttf sdl2_net sdl2_mixer
```

On Windows, build under **MSYS2/MinGW64**. `Makefile.win` expects the SDL2
headers and libraries at `C:/msys64/mingw64`; edit `INCLUDEDIR` and `LIBDIR` if
your installation lives elsewhere.

## Building

```bash
make
```

The top-level `Makefile` detects the platform and includes `Makefile.linux`,
`Makefile.win`, or `Makefile.mac`. It produces two binaries in the repository
root: **`Game`** (the client, `Game.exe` on Windows) and **`server`**.

`Makefile.mac` builds only the client. To get a server on macOS, build it by
hand:

```bash
gcc -Iinclude -o server source/server_main.c source/server.c \
    -lSDL2main -lSDL2 -lSDL2_net -lm
```

## Running

A race needs **exactly four clients** — see Known gaps. Start the server first,
then four clients with IDs 1 through 4.

```bash
./server                          # terminal 1
./Game --ip 127.0.0.1 --id 1      # terminal 2
./Game --ip 127.0.0.1 --id 2      # terminal 3
./Game --ip 127.0.0.1 --id 3      # terminal 4
./Game --ip 127.0.0.1 --id 4      # terminal 5
```

Run the binaries from the repository root — asset paths are relative to the
working directory.

| Flag | Meaning |
| --- | --- |
| `--ip <address>` | Server address. Port is always 55000. |
| `--id <1-4>` | Player slot. Each client needs a different one. |
| `--test` | Run the startup diagnostics, then continue into the game. |
| `--debug` | Verbose logging. |

**Controls**

| Key | Action |
| --- | --- |
| `W A S D` or arrow keys | Drive (switch the scheme in Options) |
| `Right Shift` | Boost, once unlocked on lap 2 |
| `Esc` | First press leaves fullscreen, second press quits |

The client redirects its own stdout and stderr to **`testlog.txt`**, so nothing
prints to the terminal. Read that file when diagnosing a failure.

## Known gaps

These are real and currently unfixed. They are tracked as issues.

- **The client does not start on Linux.** Two asset paths are written in
  lowercase (`resources/music/`, `resources/fonts/`) while the directories are
  capitalised. The font failure is fatal, so on a case-sensitive filesystem the
  window opens and closes immediately. Because stdout is redirected, the reason
  only appears in `testlog.txt`. macOS and Windows are unaffected.
- **A race needs exactly four players, not "up to four".** The server sends the
  start signal only when all four slots are active, and nothing moves until it
  arrives. Single-player and two- or three-player sessions put you on the track
  with a car that never responds to input.
- **Three buttons are invisible in the dark theme.** Their textures are
  declared and drawn but never loaded, so the back and enter buttons render as
  nothing while the dark theme is active.

## Team

Ibrahim Awad, Ahmed El Yasini, Ali Al Roudini, Ousama Sayeh Kebiche,
Hamza Osman, Malik Zihaoui.

Managed with SCRUM in Taiga.

## License

[MIT](LICENSE)
