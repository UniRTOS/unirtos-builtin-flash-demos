/*****************************************************************/ /**
* @file builtin_flash_demo.h
* @brief
* @author larson.li@quectel.com
* @date 2025-05-23
*
* @copyright Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.
* All Rights Reserved. Quectel Wireless Solution Proprietary and Confidential.
*
* @par EDIT HISTORY FOR MODULE
* <table>
* <tr><th>Date <th>Version <th>Author <th>Description"
* <tr><td>2025-05-23 <td>1.0 <td>larson.li <td> Init
* </table>
**********************************************************************/
#ifndef __BUILTIN_FLASH_DEMO_H__
#define __BUILTIN_FLASH_DEMO_H__

#include "qosa_def.h"
#include "qosa_sys.h"

/*===========================================================================
 *  Macro Definition
 ===========================================================================*/

#define CONFIG_UNIRTOS_BUILT_FLASH_DEMO_TASK_STACK_SIZE 4096                  // Demo task stack size configuration
#define UNIR_BUILT_FLASH_DEMO_TASK_PRIO                 QOSA_PRIORITY_NORMAL  // Demo task priority configuration
#define UNIR_BUILT_FLASH_SECTOR_SIZE                    4096                  // Flash sector size

/*===========================================================================
 *  Function Declaration
 ===========================================================================*/
void unir_builtin_flash_demo_init(void);

#endif /* __BUILTIN_FLASH_DEMO_H__ */
