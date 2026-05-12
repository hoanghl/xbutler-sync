# 1. Requirements

- Can run on different platforms: `macos-arm64`, mobile `arm8`
- Can synchronize files between 2 devices
- Communication is established via **HTTP** between **Server** and **Client**.

# 2. Server Endpoints

- `GET sync/`:
  - Client connects to this endpoint to synchronize files with the server.
  - Receive: `{"upload": [str], "download": [str]}`.
- `GET files/{filename}`:
  - Get a file from the server given a filename.
- `POST files/`:
  - Upload a file to the server; the file name is specified in the header.

# 3. Synchronizing Procedure for Client

1. Client scans for the files currently in the specified directory.
2. Client connects to the server (`GET sync/`) to synchronize files between client and server, then receives the list of files to send or retrieve.
3. Client sends the list of files that are not yet available on the server (`POST files/`).
4. Client fetches files that it does not yet have (`GET files/{filename}`).

# 4.Synchronizing Algorithm in Server

**Input**:

- Set of file names already in the server (set **A**).
- Set of file names already in the client (set **B**).

**Output**:

```json
{
    "download": [str],
    "upload": [str]
}
```

**Algorithm**
Apply set merging algorithm for

1. Apply left exclude join for `A` with `B` -> result is converted to vector of string and is value of field `download`
2. Apply left exclude join for `B` with `A` -> result is converted to vector of string and is value of field `upload`

# 5. Data Structure

- In both server and client, use **`unordered_map`** to maintain the key-value repository of available files.

# 6. Arguments

- **Positional arguments**:
  - `role`: [`"server"` | `"client"`].
  - `directory`: `str`.
- **Optional arguments**:
  - `server_address`: `str`.
