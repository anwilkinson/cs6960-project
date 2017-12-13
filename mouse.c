#include "types.h"
#include "x86.h"
#include "defs.h"
#include "traps.h"


// Credit: got the top 4 helper functions and some of the constant definitions
// (as well as direction for the project) from the PS/2 mouse assignment at
// http://panda.moyix.net/~moyix/cs3224/fall16/bonus_hw/bonus_hw.html

// Constants used for talking to the PS/2 controller
#define PS2CTRL             0x64    // PS/2 controller status port
#define PS2DATA             0x60    // PS/2 controller data port
#define PS2DIB              0x01    // PS/2 data in buffer
#define MOUSE_ENABLEIRQ     0x02    // The enable IRQ 12 bit in the Compaq
                                    // status byte

// Constants in the mouse movement status byte
#define MOUSE_LEFT      1           // Bit indicating the left mouse button
#define MOUSE_RIGHT     2           // Bit indicating the right mouse button
#define MOUSE_MIDDLE    4           // Bit indicating the middle mouse button
#define MOUSE_XSIGN     (1 << 4)    // Sign bit for the x-axis
#define MOUSE_YSIGN     (1 << 5)    // Sign bit for the y-axis
#define MOUSE_ALWAYS_SET 0xC0       // This bit is always set to 1

static void
mousewait(int is_read)
{
  uint tries = 100000;
  while (tries--) {
    if (is_read) {
      if (inb(PS2CTRL) & 1) return;
    }
    else {
      if ((inb(PS2CTRL) & 2) == 0) return;
    }
  }
  cprintf("Tries expired, couldn't read mouse data.\n");
}

static void
mousewait_recv(void)
{
  mousewait(1);
}

static void
mousewait_send(void)
{
  mousewait(0);
}

void
mousecmd(uchar cmd)
{
  uchar ack;
  // Sending a command
  mousewait_send();
  outb(PS2CTRL, 0xD4);
  mousewait_send();
  outb(PS2DATA, cmd);
  mousewait_recv();
  do {
    ack = inb(PS2DATA);
  } while (ack != 0xFA);
}

void
mouseinit(void)
{
  uchar status_byte;
  // Your code here
  mousewait_send();
  // enable secondary PS/2 device (the mouse)
  outb(PS2CTRL, 0xA8);
  mousewait_send();
  // select the Compaq status byte
  outb(PS2CTRL, 0x20);
  mousewait_recv();
  status_byte = inb(PS2DATA);
  // set the ENABLE_IRQ12 bit
  status_byte |= MOUSE_ENABLEIRQ;
  mousewait_send();
  // prepare to send modified status byte
  outb(PS2CTRL, 0x60);
  mousewait_send();
  // send the modified status byte over the data port
  // to enable interrupts
  outb(PS2DATA, status_byte);
  mousecmd(0xF6); // set default settings
  mousecmd(0xF4); // enable delivery interrupts in streaming mode

  // start handling IRQ_MOUSE
  //picenable(IRQ_MOUSE);
  ioapicenable(IRQ_MOUSE, 0);
}

void
mouseintr(void)
{
  // TODO
}
