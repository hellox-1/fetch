# fetch
# 📱 Phone Monitor

A lightweight system that allows a **PC to monitor selected information from an Android phone over a mobile hotspot**.

The PC-side application is written in **C** and provides a CLI for viewing phone battery information, charging status, notifications, and other permitted device information.

The Android phone runs a companion application that collects the information and exposes it through a network connection.

---

## ✨ Features

### Current / Planned Features

* 🔋 Battery percentage
* ⚡ Charging / discharging status
* 🌡️ Battery temperature
* 🔌 Battery voltage
* ❤️ Battery health
* 🔔 Android notifications
* 📱 Device information
* 🌐 Automatic phone discovery
* 🔐 Device authentication
* 📡 Real-time updates
* 💻 C-based CLI
* 🖥️ Optional `ncurses` terminal UI

---

# 🏗️ Architecture

```text
                         ANDROID PHONE
┌──────────────────────────────────────────────────────┐
│                                                      │
│                PHONE MONITOR APP                     │
│                                                      │
│   ┌────────────────┐      ┌────────────────────┐    │
│   │ Battery Manager│      │ Notification       │    │
│   │                │      │ Listener            │    │
│   └───────┬────────┘      └──────────┬─────────┘    │
│           │                          │              │
│           └────────────┬─────────────┘              │
│                        ▼                            │
│                ┌───────────────┐                   │
│                │ Event Manager │                   │
│                └───────┬───────┘                   │
│                        │                            │
│                        ▼                            │
│                ┌───────────────┐                   │
│                │ JSON Protocol │                   │
│                └───────┬───────┘                   │
│                        │                            │
│                        ▼                            │
│              ┌────────────────────┐                │
│              │ TCP / WebSocket    │                │
│              │ Server             │                │
│              └─────────┬──────────┘                │
│                        │                            │
└────────────────────────┼────────────────────────────┘
                         │
                    Wi-Fi Hotspot
                         │
                         ▼
┌──────────────────────────────────────────────────────┐
│                         PC                           │
│                                                      │
│                    C CLI CLIENT                      │
│                                                      │
│   ┌─────────────────┐                                │
│   │ Device Discovery│ ◄──── UDP                     │
│   └────────┬────────┘                                │
│            │                                         │
│            ▼                                         │
│   ┌─────────────────┐                                │
│   │ Connection      │ ◄──── TCP / WebSocket         │
│   │ Manager         │                                │
│   └────────┬────────┘                                │
│            │                                         │
│            ▼                                         │
│   ┌─────────────────┐                                │
│   │ Protocol Parser │ ◄──── JSON                     │
│   └────────┬────────┘                                │
│            │                                         │
│            ▼                                         │
│   ┌─────────────────┐                                │
│   │ Event Handler   │                                │
│   └────────┬────────┘                                │
│            │                                         │
│            ▼                                         │
│   ┌─────────────────┐                                │
│   │ CLI Renderer    │                                │
│   │ / ncurses       │                                │
│   └─────────────────┘                                │
│                                                      │
└──────────────────────────────────────────────────────┘
```

---

# 🔄 How It Works

The phone and PC are connected through the phone's Wi-Fi hotspot.

The Android application collects information from the phone and sends it to the C CLI running on the PC.

```text
Android
   │
   │ Collect information
   ▼
Battery / Notifications
   │
   ▼
Event Manager
   │
   ▼
JSON
   │
   ▼
TCP / WebSocket
   │
   │ Wi-Fi Hotspot
   ▼
C CLI
   │
   ▼
JSON Parser
   │
   ▼
Terminal
```

The PC **does not automatically have access to the phone's private data** simply because it is connected to the hotspot.

The Android application must explicitly collect and share the information.

---

# 📱 Android Application

The Android application acts as the **data provider and network server**.

## Responsibilities

The Android application will:

1. Read battery information.
2. Monitor charging state.
3. Listen for permitted Android notifications.
4. Collect device information.
5. Run a network server.
6. Serialize information into JSON.
7. Send real-time events to the PC.
8. Authenticate connected PCs.

---

## 🔋 Battery Module

The Android application can use Android's battery APIs to obtain information such as:

```text
Battery percentage
Charging status
Battery temperature
Battery voltage
Battery health
Battery technology
```

Example:

```text
BATTERY
├── level       = 73%
├── charging    = true
├── temperature = 31.4°C
├── voltage     = 4.1V
└── health      = GOOD
```

---

# 🔔 Notification Module

Notifications can be received using Android's:

```text
NotificationListenerService
```

The user must explicitly grant notification access to the application.

Example notification:

```text
WhatsApp
Rahul
Hey, are you coming?
```

The Android application converts this into a network event.

---

# 🌐 Network Architecture

The first implementation will use:

```text
TCP
```

for the main data connection.

UDP can later be used for automatic device discovery.

```text
                  Wi-Fi Hotspot

       Android                         PC
          │                             │
          │                             │
          │────── TCP Connection ──────►│
          │                             │
          │◄────── Commands ────────────│
          │                             │
          │────── JSON Events ─────────►│
          │                             │
```

Example server port:

```text
8765
```

---

# 🔎 Device Discovery

Manually specifying the phone IP is inconvenient.

Therefore, the project can implement UDP discovery.

The PC broadcasts:

```text
DISCOVER_PHONE
```

The Android application responds:

```text
PHONE_HERE|192.168.43.1|8765
```

Architecture:

```text
PC
 │
 │ UDP Broadcast
 │
 │ "DISCOVER_PHONE"
 ▼
Wi-Fi Network
 │
 ▼
Android
 │
 │ UDP Response
 │
 │ "PHONE_HERE|IP|PORT"
 ▼
PC
```

The C CLI can then automatically connect to the discovered phone.

---

# 📡 Communication Protocol

The application will use JSON messages.

## Battery Message

```json
{
  "type": "battery",
  "level": 73,
  "charging": true,
  "temperature": 31.4
}
```

## Notification Message

```json
{
  "type": "notification",
  "package": "com.whatsapp",
  "title": "Rahul",
  "text": "Hey, are you coming?"
}
```

## Device Information

```json
{
  "type": "device",
  "name": "Sanchit's Phone",
  "model": "Pixel 8",
  "android": "16"
}
```

---

# 💻 C CLI

The PC application will be written in C.

The C program will handle:

* Network connections
* Device discovery
* TCP communication
* JSON parsing
* Event handling
* CLI rendering
* Authentication
* Connection management

---

# 📂 Project Structure

Initial structure:

```text
phone-monitor/
│
├── android/
│   └── phone-monitor-app/
│
├── pc/
│   ├── src/
│   │   ├── main.c
│   │   ├── socket.c
│   │   ├── socket.h
│   │   ├── protocol.c
│   │   ├── protocol.h
│   │   ├── discovery.c
│   │   ├── discovery.h
│   │   ├── battery.c
│   │   ├── battery.h
│   │   ├── notification.c
│   │   ├── notification.h
│   │   ├── cli.c
│   │   └── cli.h
│   │
│   ├── include/
│   │
│   ├── Makefile
│   └── README.md
│
└── README.md
```

---

# 🧩 C Application Modules

## `main.c`

Entry point of the application.

Responsibilities:

```text
Parse arguments
Initialize application
Start discovery
Connect to phone
Start event loop
```

---

## `socket.c`

Handles low-level networking.

Functions will eventually include:

```c
socket();
connect();
send();
recv();
close();
```

---

## `discovery.c`

Handles UDP device discovery.

Example:

```text
DISCOVER_PHONE
```

and:

```text
PHONE_HERE|192.168.43.1|8765
```

---

## `protocol.c`

Handles communication protocol.

Responsibilities:

```text
Receive JSON
Parse JSON
Identify message type
Validate message
Create outgoing messages
```

---

## `battery.c`

Processes battery events.

Example:

```text
Battery: 73%
Charging: YES
Temperature: 31.4°C
```

---

## `notification.c`

Processes notification events.

Example:

```text
WhatsApp
Rahul: Hey, are you coming?
```

---

## `cli.c`

Responsible for terminal output.

Eventually this can be replaced or extended with `ncurses`.

---

# 🖥️ CLI Interface

Running:

```bash
./phone-monitor
```

could produce:

```text
╭──────────────────────────────────────╮
│           PHONE MONITOR              │
╰──────────────────────────────────────╯

Device      : Sanchit's Phone
Model       : Pixel 8
Connection  : Connected
IP          : 192.168.43.1

BATTERY
────────────────────────────────────────
Level       : 73%
Status      : Charging
Temperature : 31.4°C

NOTIFICATIONS
────────────────────────────────────────
[07:01] WhatsApp
        Rahul: Hey, are you coming?

[07:03] Gmail
        New email received

────────────────────────────────────────
Connected for: 00:12:31
```

---

# ⌨️ CLI Commands

The application can support several commands.

## Start monitor

```bash
phone-monitor
```

## Discover phone

```bash
phone-monitor discover
```

## Connect manually

```bash
phone-monitor connect 192.168.43.1
```

## Show battery

```bash
phone-monitor battery
```

## Show notifications

```bash
phone-monitor notifications
```

## Continuously monitor

```bash
phone-monitor watch
```

## Show device status

```bash
phone-monitor status
```

---

# 🔐 Security

The application should not expose phone information to every device connected to the hotspot.

A basic authentication protocol can be implemented.

```text
PC                           PHONE
│                              │
│          HELLO               │
│─────────────────────────────►│
│                              │
│          CHALLENGE           │
│◄─────────────────────────────│
│                              │
│          AUTH TOKEN          │
│─────────────────────────────►│
│                              │
│          ACCEPTED            │
│◄─────────────────────────────│
│                              │
│       Start data transfer    │
│◄────────────────────────────►│
```

The Android application should allow the user to approve or reject a connection.

Example:

```text
Connection Request

PC: 192.168.43.100

Allow this device?

[ Allow ] [ Deny ]
```

---

# 🛠️ Technology Stack

## Android

```text
Kotlin
Android SDK
BatteryManager
NotificationListenerService
TCP / WebSocket
JSON
```

## PC

```text
C
POSIX Sockets
TCP
UDP
pthread
JSON parser
ncurses
```

The first version can keep the C application as dependency-light as possible.

---

# 🔁 Application Flow

```text
                         START
                           │
                           ▼
                  Parse CLI arguments
                           │
                           ▼
                    Discover phone
                           │
                           ▼
                  Phone discovered?
                     /           \
                   NO             YES
                   │               │
                   ▼               ▼
                Retry          Connect
                                   │
                                   ▼
                            Authentication
                                   │
                                   ▼
                              Accepted?
                              /       \
                            NO         YES
                            │           │
                            ▼           ▼
                           Exit      Start stream
                                        │
                                        ▼
                                  Receive JSON
                                        │
                                        ▼
                                  Parse message
                                        │
                         ┌──────────────┼──────────────┐
                         ▼              ▼              ▼
                     Battery      Notification      Device
                         │              │              │
                         └──────────────┼──────────────┘
                                        ▼
                                  Update CLI
                                        │
                                        ▼
                                  Wait for event
                                        │
                                        └──────► Repeat
```

---

# 🚀 Development Roadmap

## Phase 1 — Basic C Networking

Implement:

```text
socket()
connect()
send()
recv()
close()
```

Create a simple C TCP client and server.

Test:

```text
PC → "Hello"
Android/server → "Hello from phone"
```

---

## Phase 2 — Android TCP Server

Create the Android application.

Start a TCP server:

```text
PORT = 8765
```

Allow the C client to connect.

---

## Phase 3 — Communication Protocol

Introduce JSON.

Start with:

```json
{
  "type": "hello"
}
```

Then add:

```text
battery
notification
device
```

---

## Phase 4 — Battery Monitoring

Implement:

```text
Battery percentage
Charging state
Temperature
Voltage
Health
```

Send updates to the C application.

---

## Phase 5 — Notification Monitoring

Implement:

```text
NotificationListenerService
```

Send notification events to the PC.

---

## Phase 6 — UDP Discovery

Implement:

```text
DISCOVER_PHONE
```

and:

```text
PHONE_HERE
```

The user should no longer need to manually enter the phone IP.

---

## Phase 7 — Authentication

Add:

```text
Challenge
Token
Authentication
Connection approval
```

---

## Phase 8 — Real-Time CLI

Implement an event-driven CLI.

Display:

```text
Battery
Charging
Temperature
Notifications
Connection status
```

---

## Phase 9 — Terminal UI

Add:

```text
ncurses
```

for a proper terminal dashboard.

Possible layout:

```text
┌─────────────────────────────────────────────┐
│              PHONE MONITOR                  │
├─────────────────────┬───────────────────────┤
│ BATTERY             │ DEVICE                │
│                     │                       │
│ 73%                 │ Pixel 8               │
│ ⚡ Charging         │ Android 16            │
│ 31.4°C              │ Connected             │
│                     │                       │
├─────────────────────┴───────────────────────┤
│ NOTIFICATIONS                               │
│                                             │
│ WhatsApp                                    │
│ Rahul: Hey, are you coming?                 │
│                                             │
│ Gmail                                       │
│ New email received                          │
└─────────────────────────────────────────────┘
```

---

# 🧪 Development Strategy

Build the project incrementally.

Do **not** start with the complete system.

Recommended order:

```text
C TCP Client
     ↓
C TCP Server
     ↓
Android TCP Server
     ↓
PC ↔ Android
     ↓
JSON Protocol
     ↓
Battery Data
     ↓
Notifications
     ↓
UDP Discovery
     ↓
Authentication
     ↓
Real-Time CLI
     ↓
ncurses UI
```

---

# 🔮 Future Features

Possible future additions:

```text
CPU usage
RAM usage
Storage usage
Network speed
Mobile network information
Wi-Fi information
Bluetooth status
Device uptime
Screen status
Media information
Currently playing song
Call state
SMS information with appropriate permissions
File transfer
Remote commands
Phone ping
Battery history
Battery graphs
```

Remote control should only be added with explicit Android permissions and user approval.

---

# ⚠️ Privacy and Permissions

The project should follow Android's permission and security model.

The PC cannot automatically access private phone information just because it is connected to the hotspot.

The Android application must explicitly request the required permissions.

In particular:

```text
Notifications → Notification access
Files        → Appropriate Android storage/file permissions
SMS          → SMS permissions
Calls        → Appropriate phone permissions
```

The application should only send information that the user has explicitly enabled.

---

# 🎯 Project Goal

The goal is to build a lightweight **Android-to-PC device monitoring system** where:

```text
Android
   │
   │ collects device information
   ▼
Network Protocol
   │
   │ Wi-Fi hotspot
   ▼
C CLI
   │
   ▼
Terminal
```

The project is intended to provide practical experience with:

* C programming
* Linux/POSIX sockets
* TCP/IP
* UDP
* Client-server architecture
* Network discovery
* JSON protocols
* Event-driven programming
* Multithreading
* Android APIs
* Inter-process/network communication
* Terminal UI development
* Network security

---

# 📜 License

Choose a license when the project is ready for public release.

For example:

```text
MIT License
```

---

# 👨‍💻 Project Status

```text
🚧 Under Development
```

The project will be developed incrementally, starting with a basic **C TCP client/server implementation** and gradually adding Android integration, battery monitoring, notification streaming, discovery, authentication, and terminal UI.
