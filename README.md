# ProcView

ProcView is a lightweight Linux system monitor written in C.

It reads system and process information directly from the Linux `/proc` filesystem and displays CPU usage, memory usage, and the top running processes in real time.

## Features

- Overall CPU usage monitoring
- Overall memory usage monitoring
- Per-process CPU usage
- Per-process memory usage
- Sort processes by CPU or memory usage
- Display the top 10 processes
- Configurable refresh interval
- Command-line interface
- Real-time terminal refresh

## Usage

Run with default settings:

```bash
./procview
```
Sort by memory usage:

```bash
./procview --sort memory 
```
Sort by CPU usage:

```bash
./procview --sort cpu
```
Set the refresh interval:

```bash
./procview --interval 5
```

Combine multiple options:

```bash
./procview --sort cpu --interval 5
```

Show help:

```bash
./procview --help
```

Stop the monitor with:

```text
Ctrl + C
```

## Build

Compile the project with:

```bash
make
```

Run ProcView:

```bash
./procview 
```

Remove the compiled executable:

```bash
make clean
```

## Example Output

```text
ProcView - Linux System Monitor

CPU Usage: 2.5%
Memory Usage: 0.99 GB / 4.19 GB
Refresh Interval: 2 seconds

PID      PROCESS                       CPU     MEMORY
2208     gnome-shell                   1.5%     373.4 MB
2904     mutter-x11-fram               0.0%      97.2 MB
2805     gsd-xsettings                 0.0%      81.0 MB
2667     Xwayland                      0.0%      68.8 MB
2806     gjs                           0.0%      59.7 MB
```

## Project Structure

```
ProcView/
├── include/
│   ├── cpu.h
│   ├── memory.h
│   └── process.h
├── src/
│   ├── cpu.c
│   ├── main.c
│   ├── memory.c
│   └── process.c
├── tests/
├── Makefile
├── README.md
└── LICENSE
```

## How It Works

ProcView reads system information directly from the Linux /proc virtual filesystem.

/proc/stat is used to calculate overall CPU usage.
/proc/meminfo is used to calculate system memory usage.
/proc/<PID>/stat is used to calculate per-process CPU usage.
/proc/<PID>/status is used to read per-process memory usage.
/proc/<PID>/comm is used to read process names.

CPU usage values are cumulative in Linux, so ProcView takes two measurements over a time interval and calculates the difference between them.

For per-process monitoring, ProcView matches processes between measurements using their PID and calculates CPU usage from the change in user and system CPU time.

## Command-Line Options

| Option          | Description                           |
| --------------- | ------------------------------------- |
| `--sort memory` | Sort processes by memory usage        |
| `--sort cpu`    | Sort processes by CPU usage           |
| `--interval N`  | Set the refresh interval to N seconds |
| `--help`        | Display usage information             |

Options can be combined:

```bash
./procview --sort cpu --interval 5
```

## Technologies

- C
- Linux
- `/proc` virtual filesystem
- GCC
- Make
- Git

## License 

This project is licensed under the MIT License.
