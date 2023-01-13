/** @file
 *  @brief HRS Service sample
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#define LOG_LEVEL CONFIG_BT_HRS_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ws);


#define GATT_PERM_READ_MASK     (BT_GATT_PERM_READ | \
				 BT_GATT_PERM_READ_ENCRYPT | \
				 BT_GATT_PERM_READ_AUTHEN)
#define GATT_PERM_WRITE_MASK    (BT_GATT_PERM_WRITE | \
				 BT_GATT_PERM_WRITE_ENCRYPT | \
				 BT_GATT_PERM_WRITE_AUTHEN)

#ifndef CONFIG_BT_HRS_DEFAULT_PERM_RW_AUTHEN
#define CONFIG_BT_HRS_DEFAULT_PERM_RW_AUTHEN 0
#endif
#ifndef CONFIG_BT_HRS_DEFAULT_PERM_RW_ENCRYPT
#define CONFIG_BT_HRS_DEFAULT_PERM_RW_ENCRYPT 0
#endif
#ifndef CONFIG_BT_HRS_DEFAULT_PERM_RW
#define CONFIG_BT_HRS_DEFAULT_PERM_RW 0
#endif

#define HRS_GATT_PERM_DEFAULT (						\
	CONFIG_BT_HRS_DEFAULT_PERM_RW_AUTHEN ?				\
	(BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN) :	\
	CONFIG_BT_HRS_DEFAULT_PERM_RW_ENCRYPT ?				\
	(BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT) :	\
	(BT_GATT_PERM_READ | BT_GATT_PERM_WRITE))			\

static uint8_t ws_blsc;

static void wsmc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("WS notifications %s", notif_enabled ? "enabled" : "disabled");
}

static ssize_t read_blsc(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &ws_blsc,
				 sizeof(ws_blsc));
}

/* Weight Scale Service Declaration */
BT_GATT_SERVICE_DEFINE(ws_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_WSS),
	BT_GATT_CHARACTERISTIC(BT_UUID_WSS_MEASUREMENT, BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_NONE, NULL, NULL, NULL),
	BT_GATT_CCC(wsmc_ccc_cfg_changed,
		    HRS_GATT_PERM_DEFAULT),
	BT_GATT_CHARACTERISTIC(BT_UUID_WSS_FEATURE, BT_GATT_CHRC_READ,
			       HRS_GATT_PERM_DEFAULT & GATT_PERM_READ_MASK,
			       read_blsc, NULL, NULL),

);

static int ws_init(const struct device *dev)
{
	ARG_UNUSED(dev);

	ws_blsc = 0x01;

	return 0;
}

int bt_ws_notify(int32_t weight)
{
	int rc;
	static uint8_t wsm[4];

	wsm[0] = 0x00; /* uint8, sensor contact */
	wsm[1] = (weight >> 16) & 0xFF;
	wsm[2] = (weight >> 8) & 0xFF;
	wsm[3] = weight & 0xFF;

	LOG_DBG("Notify: top: %x, mid: %x , bottom: %x ", wsm[1],wsm[2],wsm[3]);

	rc = bt_gatt_notify(NULL, &ws_svc.attrs[1], &wsm, sizeof(wsm));

	return rc == -ENOTCONN ? 0 : rc;
}

SYS_INIT(ws_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
