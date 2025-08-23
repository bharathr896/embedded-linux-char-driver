#ifndef _MSG_BOARD_UAPI_H
#define _MSG_BOARD_UAPI_H

#include <linux/ioctl.h>

#define MSG_BOARD_MAGIC  'M'

// IOCTL commands
//#define IOCTL_GET_MSG_COUNT   _IOR(MSG_BOARD_MAGIC, 1, int)
#define MSGB_GET_OPEN_COUNT   _IOR(MSG_BOARD_MAGIC, 1, int)

#define MSGB_CLEAR_BOARD      _IO(MSG_BOARD_MAGIC,  2)
#define MSGB_SET_MESSAGE      _IOW(MSG_BOARD_MAGIC, 3, char[128])

#endif