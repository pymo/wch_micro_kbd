#ifndef __TEST_DTM_PORT_H
#define __TEST_DTM_PORT_H

#include "CH58x_common.h"


void test_dtm_usb_init(void);
int test_dtm_tx( uint8_t *dtm_cmd, size_t len);


#endif /* __TEST_DTM_PORT_H */
