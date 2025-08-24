# Message Board Kernel Module

This project demonstrates a simple Linux character device driver that works like a message board. It supports IOCTL calls to set, get, and clear messages, as well as retrieve the open count.

---

## Features

- Maintains an in-kernel message buffer.
- Provides IOCTL commands for:
  - **Set Message** – Write a message to the board.
  - **Get Message** – Read the current message.
  - **Clear Board** – Clear the stored message.
  - **Get Open Count** – Get how many times the device has been opened.
- Comes with a user-space program to interact with the device.

---

## IOCTL Commands

| Command               | Description                |
|-----------------------|---------------------------|
| `MSGB_SET_MESSAGE`    | Write a new message       |
| `MSGB_GET_MESSAGE`    | Read the current message  |
| `MSGB_CLEAR_BOARD`    | Clear the message board   |
| `MSGB_GET_OPEN_COUNT` | Get open count of device  |

---

## How It Works

- The kernel module creates a **/dev/msg_board** device.
- The message buffer has a fixed size (default: 128 bytes).
- All interactions happen through IOCTL commands from the user application.
- Mutex locking is used for clearing and reading messages to avoid race conditions.

---

## Building & Running

1. **Build the Kernel Module**
   ```bash
   make
   ```
2. **Insert the module**
    ```bash
    sudo insmod msg_board.ko
    ```
3. **Run user application**
    ```bash
    gcc user/test.c -o test -Iinclude
    ```

