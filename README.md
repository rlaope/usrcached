# usrcached

`usrcached` is a simple in-memory cache server project for Windows, built with the Win32 API and Winsock2. It offers CRUD operations over HTTP for storing and retrieving `key`-to-`user_id` mappings.

---

## Project Structure

```
usrcached/
戍式式 include/
弛   戍式式 cache.h      # Cache logic interface
弛   戌式式 http.h       # HTTP request/response utilities
戍式式 src/
弛   戍式式 cache.c      # Cache logic implementation (CRITICAL_SECTION)
弛   戍式式 http.c       # HTTP parsing and response (Winsock2 send/recv)
弛   戌式式 server.c     # Entry point: socket setup, thread creation, routing
戌式式 Makefile         # Build script (MinGW)
```

---

## Features

1. **Cache Management**

   * `cache_init()` / `cache_cleanup()` for initializing and cleaning the internal CRITICAL\_SECTION and linked list.
2. **CRUD Operations**

   * `cache_put(key, user_id)` ⊥ create (201) or update (200)
   * `cache_find(key)` ⊥ retrieve (200) or 404 if not found
   * `cache_delete(key)` ⊥ delete (200) or 404 if not found
3. **HTTP Server**

   * Winsock2 initialization (`WSAStartup`) and cleanup (`WSACleanup`)
   * TCP socket binding and listening
   * Per-connection handler threads via `CreateThread`
   * Lightweight HTTP parsing (request line + body separated by `\r\n\r\n`)

---

## Building and Running

### Prerequisites

* Windows with MinGW (or MSYS2)
* `gcc`, `make`

### Build

Run in project root:

```bash
make
```

Generates `server.exe`.

### Run

```bash
./server.exe
```

Default port: **8080**

---

## API Examples

```bash
# Create/Update
curl -X POST http://localhost:8080/cache/alice -d "user_id=1001"
# Read
curl        http://localhost:8080/cache/alice
# Delete
curl -X DELETE http://localhost:8080/cache/alice
```

Sample JSON responses:

```json
{ "user_id": 1001 }
{ "status": "created" }
{ "status": "updated" }
{ "error": "Not found" }
```

---

## Extension Ideas

* TTL (time-to-live) support
* LRU/LFU eviction policies
* Persistent storage backend (file or database)
* HTTPS support

---

## License

MIT
