# dclog

A small Qt6 desktop app for viewing Docker container logs.

- **Left panel** — table of all running containers (Name, Image, Status), refreshed every 2 seconds via `docker ps`.
- **Right panel** — live-streamed logs for whichever container you click, via `docker logs -f`.

No Docker SDK dependency — it shells out to the `docker` CLI, so it works wherever `docker` is installed and configured.

## Requirements

- Qt 6.5+ (Core, Widgets)
- CMake 3.19+
- A C++ compiler with C++17 support
- `docker` CLI available on `PATH` and able to reach a Docker daemon

## Build

```
cmake -S . -B build
cmake --build build
```

## Run

```
./build/dclog
```

Click a row in the left-hand table to start streaming that container's logs in the right-hand panel.
