# UDP Ping Client-Server

A UDP-based ping utility written in C that simulates packet loss and measures round-trip time (RTT). Built as part of **CSCE 3600 – Systems Programming** at the University of North Texas.

## Features

- UDP socket programming (client/server architecture)
- Configurable simulated packet loss on the server side
- RTT measurement using `clock_gettime` (nanosecond precision)
- Per-ping timeout using `select()`
- Summary stats: packets sent/received/lost, min/avg/max RTT

## Files

| File | Description |
|---|---|
| `minor4svr.c` | UDP server — listens for pings, simulates packet loss, echoes replies |
| `minor4cli.c` | UDP client — sends pings, measures RTT, prints summary stats |
| `Makefile` | Build both binaries with `make` |

## Build

```bash
make
```

Or manually:

```bash
gcc -Wall -o server minor4svr.c
gcc -Wall -o client minor4cli.c
```

## Usage

**Start the server** (in one terminal):
```bash
./server <port> <loss_rate>
# Example: 30% packet loss on port 8080
./server 8080 30
```

**Run the client** (in another terminal):
```bash
./client <server_ip> <port> <num_pings> <timeout_ms>
# Example: 10 pings to localhost with 2000ms timeout
./client 127.0.0.1 8080 10 2000
```

## Example Output

**Server:**
```
UDP Ping Server listening on port 8080 (loss rate: 30%)
[REPLIED] Packet from 127.0.0.1:54321 | Seq: PING 1
[DROPPED] Packet from 127.0.0.1:54321 | Seq: PING 2
[REPLIED] Packet from 127.0.0.1:54321 | Seq: PING 3
```

**Client:**
```
UDP Ping Client
Target: 127.0.0.1:8080 | Pings: 10 | Timeout: 2000ms

Reply from 127.0.0.1 | seq=1 | RTT=0.412ms
Ping seq=2 | REQUEST TIMEOUT
Reply from 127.0.0.1 | seq=3 | RTT=0.387ms
...

--- Ping Statistics ---
Transmitted: 10 | Received: 7 | Lost: 3 (30.0% loss)
RTT min/avg/max: 0.312 / 0.441 / 0.698 ms
```

## Concepts Demonstrated

- UDP socket creation and binding (`socket`, `bind`, `sendto`, `recvfrom`)
- Non-blocking I/O with timeout (`select`)
- High-resolution timing (`clock_gettime`)
- Packet loss simulation via random number generation

## Author

**Yeshwanth Salapu**
B.S. Computer Science — University of North Texas
[LinkedIn](https://www.linkedin.com/in/yeshwanth-salapu-a257b7291/) | [GitHub](https://github.com/Ysalapu24)
