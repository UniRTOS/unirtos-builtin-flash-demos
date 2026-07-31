/*****************************************************************/ /**
* @file builtin_flash_demo.c
* @brief
* @author larson.li@quectel.com
* @date 2025-05-23
*
* @copyright Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.
* All Rights Reserved. Quectel Wireless Solution Proprietary and Confidential.
*
* @par EDIT HISTORY FOR MODULE
* <table>
* <tr><th>Date <th>Version <th>Author <th>Description
* <tr><td>2025-05-23 <td>1.0 <td>larson.li <td> Init
* </table>
**********************************************************************/
#include "qosa_sys.h"
#include "qosa_built_in_flash.h"
#include "qosa_def.h"
#include "qosa_log.h"
#include "qosa_rtc.h"
#include "builtin_flash_demo.h"
#include "unirtos_app_init_registry.h"

/*===========================================================================
 *  Macro Definition
 ===========================================================================*/
#define QOS_LOG_TAG LOG_TAG_DEMO

/*===========================================================================
 *  Variate
 ===========================================================================*/
static qosa_task_t g_unir_built_flash_demo_task = QOSA_NULL;
#define UNIR_FOTA_BACKUP_ADDR 0x14000  // Specified erase address (non-code erasable area)

/*===========================================================================
 *  Static API Functions
 ===========================================================================*/
/**
 * @brief Built-in Flash read/write test function for cyclically executing Flash erase, write, and read operations, and verifying data consistency.
 *
 * This function continuously performs Flash operation tests, each test includes the following steps:
 * 1. Erase specified Flash area;
 * 2. Write fixed value (0xA5) to Flash;
 * 3. Read data from Flash;
 * 4. Verify if read data matches written data;
 * 5. Record and print time consumption for each stage;
 * 6. Cycle through the above process.
 *
 * @param ctx Task context pointer, unused.
 *
 * @note If any operation fails (such as erase, write, or read failure, data inconsistency, etc.), exit the loop and release resources.
 */
static void unir_built_flash_demo_process(void *ctx)
{
    int           ret = 0;
    qosa_uint8_t *data_buf = QOSA_NULL;
    qosa_uint8_t *data_buf_tmp = QOSA_NULL;
    int           i = 0;
    qosa_uint32_t erase_time = 0;
    qosa_uint32_t write_time = 0;
    qosa_uint32_t read_time = 0;
    qosa_uint32_t timeval = 0;

    // Delay 5s to prevent log loss during boot
    qosa_task_sleep_ms(5000);
    // Allocate 4k memory for temporary flash read/write data storage
    data_buf = qosa_malloc(UNIR_BUILT_FLASH_SECTOR_SIZE);
    if (data_buf == QOSA_NULL)
    {
        QLOGE("malloc data_buf failed!\r\n");
        return;
    }
    // Allocate 4k memory for data consistency comparison
    data_buf_tmp = qosa_malloc(UNIR_BUILT_FLASH_SECTOR_SIZE);
    if (data_buf_tmp == QOSA_NULL)
    {
        if (data_buf)
        {
            qosa_free(data_buf);
            data_buf = QOSA_NULL;
        }
        QLOGE("malloc data_buf_tmp failed!\r\n");
        return;
    }

    do {
        // Erase 4k flash area and measure time consumption
        timeval = qosa_get_system_tick_cnt();
        ret = qosa_builtin_flash_erase(UNIR_FOTA_BACKUP_ADDR, UNIR_BUILT_FLASH_SECTOR_SIZE);
        if (ret != 0)
        {
            QLOGE("erase flash fail");
            break;
        }
        erase_time = qosa_get_system_tick_cnt() - timeval;

        // Fill data to be written (0xA5), perform write operation, and measure time consumption
        qosa_memset(data_buf, 0xA5, UNIR_BUILT_FLASH_SECTOR_SIZE);
        timeval = qosa_get_system_tick_cnt();
        ret = qosa_builtin_flash_write(UNIR_FOTA_BACKUP_ADDR, data_buf, UNIR_BUILT_FLASH_SECTOR_SIZE);
        if (ret != 0)
        {
            QLOGE("write flash fail");
            break;
        }
        write_time = qosa_get_system_tick_cnt() - timeval;

        // Clear temporary storage area to 0, read flash data, and measure time consumption
        qosa_memset(data_buf, 0, UNIR_BUILT_FLASH_SECTOR_SIZE);
        timeval = qosa_get_system_tick_cnt();
        ret = qosa_builtin_flash_read(UNIR_FOTA_BACKUP_ADDR, data_buf, UNIR_BUILT_FLASH_SECTOR_SIZE);
        if (ret != 0)
        {
            QLOGE("read flash fail");
            break;
        }
        read_time = qosa_get_system_tick_cnt() - timeval;
        qosa_memset(data_buf_tmp, 0xA5, UNIR_BUILT_FLASH_SECTOR_SIZE);

        // Compare data consistency - check if read data matches written data
        if (qosa_memcmp(data_buf, data_buf_tmp, UNIR_BUILT_FLASH_SECTOR_SIZE) != 0)
        {
            QLOGE("flash data is not same");
            break;
        }
        else
        {
            QLOGD("flash data is same");
        }

        // Update test counter and print current test result
        i++;
        QLOGD("flash erase cnt %d time:%d ms,write time:%d ms,read time:%d ms", i, erase_time, write_time, read_time);
        // Wait 100ms before continuing to next test round
        qosa_task_sleep_ms(100);
    } while (1);

    // Reaching here indicates an exception occurred - release corresponding memory and print error log
    if (data_buf)
    {
        qosa_free(data_buf);
        data_buf = QOSA_NULL;
    }

    if (data_buf_tmp)
    {
        qosa_free(data_buf_tmp);
        data_buf_tmp = QOSA_NULL;
    }
    QLOGV("unir_demo_builtin_flash_init test failed please check !!!");
}

void unir_builtin_flash_demo_init(void)
{
    QLOGV("enter UniRTOS built flash DEMO !!!");
    if (g_unir_built_flash_demo_task == QOSA_NULL)
    {
        qosa_task_create(
            &g_unir_built_flash_demo_task,
            CONFIG_UNIRTOS_BUILT_FLASH_DEMO_TASK_STACK_SIZE,
            UNIR_BUILT_FLASH_DEMO_TASK_PRIO,
            "builtin_flash_demo",
            unir_built_flash_demo_process,
            QOSA_NULL,
            1
        );
    }
}
UNIRTOS_APP_EXPORT(320, "builtin_flash_demo", unir_builtin_flash_demo_init);