#ifndef __KERN_HELLO__
#define __KERN_HELLO__

#include <asm/ioctl.h>

typedef enum {
    E_DEMO_CMD_OPEN,
    E_DEMO_CMD_CLOSE,
    E_DEMO_CMD_GET,
    E_DEMO_CMD_PUT,
    E_DEMO_CMD_MAX,
} CmdType_e;

#define DEMO_TYPE   'A'

#define IOCTL_DEMO_CMD_OPEN  _IOWR(DEMO_TYPE, E_DEMO_CMD_OPEN, unsigned int)
#define IOCTL_DEMO_CMD_CLOSE _IOWR(DEMO_TYPE, E_DEMO_CMD_CLOSE, unsigned int)
#define IOCTL_DEMO_CMD_GET   _IOR(DEMO_TYPE, E_DEMO_CMD_GET, unsigned int)
#define IOCTL_DEMO_CMD_PUT   _IOW(DEMO_TYPE, E_DEMO_CMD_PUT, unsigned int)

#endif /* end of __KERN_HELLO__ */

