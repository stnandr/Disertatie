#include "MbClient.h"

// STL
#include <array>
#include <string>

// ESP Deps
#include "protocol_examples_common.h"
#include <string.h>
#include <sys/queue.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_defaults.h"
#include "esp_netif_net_stack.h"
#include "esp_wifi_netif.h"
#include "esp_netif_br_glue.h"
#include "esp_netif_ppp.h"
#include "esp_netif_sntp.h"
#include "esp_mac.h"
#include "mdns.h"
#include "protocol_examples_common.h"
#include "modbus_params.h"      // for modbus parameters structures
#include "mbcontroller.h"
#include "sdkconfig.h"
#include "esp_log.h"   
#include "esp_modbus_common.h"
#include "esp_modbus_master.h"
#include "lwip/err.h"           //light weight ip packets error handling
#include "lwip/sys.h"           //system applications for light weight ip apps
#include "lwip/sockets.h"
#include <lwip/netdb.h>
#include <lwip/netif.h>

typedef esp_err_t error_t;
typedef void* VOID_PTR;

namespace
{
    constexpr int PORT = 502;
    
    static const char* TAG = "Modbus socket";

    enum eDeviceAddr{
        DEVICE1 = 0x7E,
        COUNT
    };

    enum eCID {
        NUM,
        VER,
        DEV,
        TMP
    };

    const char* IP_TABLE[3] = {
        "192.168.1.101"
    };

}

MbClient::MbClient()
{
    VOID_PTR handler = NULL;
    error_t error = mbc_master_init_tcp(&handler);

    if (handler == NULL || error != ESP_OK)
        ESP_LOGE(TAG, "Modbus controller initialization fail.");

    mb_parameter_descriptor_t deviceParams[] =
    {
        {
            eCID::NUM,
            "Test",
            "Unit",
            eDeviceAddr::DEVICE1,
            MB_PARAM_HOLDING,
            0x4BFD,
            2,
            0,
            PARAM_TYPE_U16,
            static_cast<mb_descr_size_t>(4),
            0,
            PAR_PERMS_READ_WRITE_TRIGGER
        }
    };

    uint16_t deviceParamSize = (sizeof(deviceParams) / sizeof(deviceParams[0]));
    ESP_ERROR_CHECK(mbc_master_set_descriptor(&deviceParams[0], deviceParamSize));

    esp_netif_t* netif = NULL;
    netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    mb_communication_info_t communicationInfo = {
        .ip_mode = MB_MODE_TCP,
        .ip_port = 502,
        .ip_addr_type = MB_IPV4,
        .ip_addr = (void*)IP_TABLE,
        .ip_netif_ptr = netif
    };

    ESP_ERROR_CHECK(mbc_master_setup(static_cast<VOID_PTR>(&communicationInfo)));

    error = mbc_master_start();

    if (error != ESP_OK) 
        ESP_LOGE(TAG, "MB controller start fail, err = %x.", error);
}

MbClient::~MbClient()
{
    ESP_ERROR_CHECK(mbc_master_destroy());
}

void MbClient::SendReceiveRequest()
{
    mb_param_request_t request;
    request.slave_addr =  eDeviceAddr::DEVICE1;
    request.command    = 0x03;
    request.reg_start  = 0x4BFE;
    request.reg_size   = 0x02;
    
    uint32_t data = 0xFFFF;

    mbc_master_send_request(&request, &data);
    ESP_LOGI(TAG, "%lu", data);
    
}

void MbClient::ProcessRequest()
{
    // Unimplemented
}
