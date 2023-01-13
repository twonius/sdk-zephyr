/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_WSS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_WSS_H_

/**
 * @brief Weight Scale Service (WSS)
 * @defgroup bt_wsss Weight Scale Service (WSS)
 * @ingroup bluetooth
 * @{
 *
 * [Experimental] Users should note that the APIs can change
 * as a part of ongoing development.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Notify weight measurement.
 *
 * This will send a GATT notification to all current subscribers.
 *
 *  @param weight The heartrate measurement in beats per minute.
 *
 *  @return Zero in case of success and error code in case of error.
 */
int bt_ws_notify(int32_t weight);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_HRS_H_ */
