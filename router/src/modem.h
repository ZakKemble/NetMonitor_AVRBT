/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */#ifndef MODEM_H_#define MODEM_H_void modem_init(void);void modem_update(void);s_netUpDown modem_get(void);void modem_end(void);#endif /* MODEM_H_ */