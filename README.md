# xbutler-sync

## What Is the Project?

`xbutler-sync` is a file synchronization application that operates in a client-server model. Its purpose is to synchronize files between a server machine and a client machine by comparing both sides, merging their file lists, and transferring the necessary files in both directions.

## Fundamental Specifications

`xbutler-sync` supports two modes: server mode and client mode.

In server mode, the application first scans all files on the server to build a complete file list. After that, it waits for incoming connections from clients.

In client mode, the application first scans all files in the specified local folder. It then connects to a specific sync server using the server's IP address.

Once the client connects, the synchronization process begins. The sync logic is merge-oriented, meaning it tries to produce a unified file list from both the server and the client. Based on that comparison, the system decides which files the server should send to the client and which files the client should send back to the server.

After that, file transfer begins on both sides. The server sends the required files to the client, and the client sends the required files back to the server.

### Execution Behavior

- If the application is started without a server IP, it automatically runs in server mode.
- If the application is started with a server IP, it runs in client mode and connects to that server.
