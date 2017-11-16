
#define PF2   (*((volatile uint32_t *)0x40025010))

#define PF4   (*((volatile uint32_t *)0x40025040))
//#define PF3   (*((volatile uint32_t *)0x40025020))
//#define PF1   (*((volatile uint32_t *)0x40025008))
//#define PF0   (*((volatile uint32_t *)0x40025004))

void SwitchInit(void);

void Heartbeat_Init(void);

uint8_t GetSwitch(void);
