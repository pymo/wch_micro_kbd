#ifndef __RF_CONFIG_H
#define __RF_CONFIG_H


/* If defined, this must be dongle */
// #define CONFIG_RF_DONGLE

/* Define the device as a keyboard */
#define CONFIG_RF_DEV_KBD

/* Define the device as a mouse */
// #define CONFIG_RF_DEV_MOUSE

#if (!defined CONFIG_RF_DEV_KBD && !defined CONFIG_RF_DEV_MOUSE && !defined CONFIG_RF_DONGLE)
#error "Unknown device !"
#endif

#if (defined CONFIG_RF_DEV_KBD && defined CONFIG_RF_DEV_MOUSE && !defined CONFIG_RF_DONGLE)
#error "Device conflict !"
#endif

#if ((defined CONFIG_RF_DEV_KBD || defined CONFIG_RF_DEV_MOUSE) && defined CONFIG_RF_DONGLE)
#error "Device conflict !"
#endif

/**
 * Define to enable frequency hopping function, 
 * if disabled, it will communicate on the default channel
 */
#define CONFIG_RF_HOP

#ifdef CONFIG_RF_HOP
#define CONFIG_RF_CHANNEL_MAP       {0, 8, 15, 23, 30, 36}
#define CONFIG_RF_CHANNEL_NUM       6
#else
#define CONFIG_RF_CHANNEL_MAP       {}
#define CONFIG_RF_CHANNEL_NUM       1
#endif

/**
 * Define to enable pair function,
 * if disabled, it wiil communicate on the default access address.
 */
#define CONFIG_RF_PAIR

/** Define to enable gpio debug, 
 * the communication process can be captured by a logic analyzer.
 * ATTENTION: If defined, the keyscan fucntion should be disable!
 */
// #define CONFIG_RF_DEBUG_GPIO

/** Define to enable debug during pairing process. */
#define CONFIG_RF_PAIR_DEBUG


/** 
 * Configure the timeout for the pairing phase, 
 * it starts timing at the beginning of power up.
 */
#define CONFIG_RF_DEV_PAIR_TO           (10 * 1000)

/** Configure the receive window size on the device side. */
#define CONFIG_RF_DEV_RCV_WINDOW_US     100

/** Configure the communication interval */
#define CONFIG_RF_SYNC_UNIT_MS          1

/** Configure the heartbeat packet sending interval, 
 * If the internal 32K is selected, its value should not be too small. 
*/
#define CONFIG_RF_HB_INTERVAL_MS        100

#endif /* __RF_CONFIG_H */
