/* generated thread source file - do not edit */
#include "user_thread.h"

TX_THREAD user_thread;
void user_thread_create(void);
static void user_thread_func(ULONG thread_input);
/** Alignment requires using pragma for IAR. GCC is done through attribute. */
#if defined(__ICCARM__)
#pragma data_alignment = BSP_STACK_ALIGNMENT
#endif
static uint8_t user_thread_stack[16384] BSP_PLACE_IN_SECTION(".stack.user_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
#if defined(__ICCARM__)
#define g_dns0_err_callback_WEAK_ATTRIBUTE
#pragma weak g_dns0_err_callback  = g_dns0_err_callback_internal
#elif defined(__GNUC__)
#define g_dns0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_dns0_err_callback_internal")))
#endif
void g_dns0_err_callback(void * p_instance, void * p_data)
g_dns0_err_callback_WEAK_ATTRIBUTE;
NX_DNS g_dns0;
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_dns0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_dns0_err_callback_internal(void * p_instance, void * p_data);
void g_dns0_err_callback_internal(void * p_instance, void * p_data)
{
    /** Suppress compiler warning for not using parameters. */
    SSP_PARAMETER_NOT_USED (p_instance);
    SSP_PARAMETER_NOT_USED (p_data);

    /** An error has occurred. Please check function arguments for more information. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
}
#if defined(__ICCARM__)
#define g_sf_comms0_err_callback_WEAK_ATTRIBUTE
#pragma weak g_sf_comms0_err_callback  = g_sf_comms0_err_callback_internal
#elif defined(__GNUC__)
#define g_sf_comms0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_comms0_err_callback_internal")))
#endif
void g_sf_comms0_err_callback(void * p_instance, void * p_data)
g_sf_comms0_err_callback_WEAK_ATTRIBUTE;
/***********************************************************************************************************************
 * USB Communications Framework (SF_EL_UX_COMMS) Instance for g_sf_comms0
 **********************************************************************************************************************/
sf_el_ux_comms_instance_ctrl_t g_sf_comms0_instance_ctrl =
{ .p_cdc = NULL, };

/* Instance structure to use this module. */
const sf_comms_instance_t g_sf_comms0 =
{ .p_ctrl = &g_sf_comms0_instance_ctrl, .p_cfg = NULL, .p_api = &g_sf_el_ux_comms_on_sf_comms };

/***********************************************************************************************************************
 * USBX Device CDC-ACM Instance Activate Callback function required for g_sf_comms0
 **********************************************************************************************************************/
VOID ux_cdc_device0_instance_activate(VOID *cdc_instance)
{
    /* Save the CDC instance for g_sf_comms0. */
    g_sf_comms0_instance_ctrl.p_cdc = (UX_SLAVE_CLASS_CDC_ACM *) cdc_instance;

    /* Inform the CDC instance activation event for g_sf_comms0. */
    if (NULL != g_sf_comms0.p_ctrl)
    {
        sf_el_ux_comms_instance_ctrl_t * p_ux_comms_ctrl = (sf_el_ux_comms_instance_ctrl_t *) g_sf_comms0.p_ctrl;

        /* Check if the semaphore for g_sf_comms0 is ready. */
        if (0x53454D41 != p_ux_comms_ctrl->semaphore.tx_semaphore_id)
        {
            tx_semaphore_create (&p_ux_comms_ctrl->semaphore, (CHAR *) "UX_COMMS_SEMAPHORE", 0);
        }

        /* Put a semaphore if the instance for g_sf_comms0 to inform CDC instance is ready. */
        tx_semaphore_ceiling_put (&p_ux_comms_ctrl->semaphore, 1);
    }
    return;
}
/***********************************************************************************************************************
 * USBX Device CDC-ACM Instance Deactivate Callback function required for g_sf_comms0
 **********************************************************************************************************************/
VOID ux_cdc_device0_instance_deactivate(VOID *cdc_instance)
{
    SSP_PARAMETER_NOT_USED (cdc_instance);

    /* Reset the CDC instance for g_sf_comms0. */
    g_sf_comms0_instance_ctrl.p_cdc = UX_NULL;

    return;
}
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - voidg_sf_comms0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_comms0_err_callback_internal(void * p_instance, void * p_data);
extern char gStopConsole;
void g_sf_comms0_err_callback_internal(void * p_instance, void * p_data)
{
	gStopConsole = 1;
}
/*******************************************************************************************************************//**
 * @brief     This is sf comms initialization function. User Can call this function in the application if required with the below mentioned prototype.
 *            - void sf_comms_init0(void)
 **********************************************************************************************************************/
void sf_comms_init0(void)
{
    ssp_err_t ssp_err_g_sf_comms0;
    /** Open USB Communications Framework */
    ssp_err_g_sf_comms0 = g_sf_comms0.p_api->open (g_sf_comms0.p_ctrl, g_sf_comms0.p_cfg);
    if (UX_SUCCESS != ssp_err_g_sf_comms0)
    {
        g_sf_comms0_err_callback ((void *) &g_sf_comms0, &ssp_err_g_sf_comms0);
    }
}
#if defined(__ICCARM__)
#define g_dhcp_client0_err_callback_WEAK_ATTRIBUTE
#pragma weak g_dhcp_client0_err_callback  = g_dhcp_client0_err_callback_internal
#elif defined(__GNUC__)
#define g_dhcp_client0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_dhcp_client0_err_callback_internal")))
#endif
void g_dhcp_client0_err_callback(void * p_instance, void * p_data)
g_dhcp_client0_err_callback_WEAK_ATTRIBUTE;
NX_DHCP g_dhcp_client0;
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_dhcp_client0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_dhcp_client0_err_callback_internal(void * p_instance, void * p_data);
void g_dhcp_client0_err_callback_internal(void * p_instance, void * p_data)
{
    /** Suppress compiler warning for not using parameters. */
    SSP_PARAMETER_NOT_USED (p_instance);
    SSP_PARAMETER_NOT_USED (p_data);

    /** An error has occurred. Please check function arguments for more information. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
}
#if (1) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_timer0) && !defined(SSP_SUPPRESS_ISR_GPT0)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 0);
#endif
#endif
static gpt_instance_ctrl_t g_timer0_ctrl;
static const timer_on_gpt_cfg_t g_timer0_extend =
{ .gtioca =
{ .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .gtiocb =
  { .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW } };
static const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_PERIODIC, .period = 10, .unit = TIMER_UNIT_PERIOD_MSEC, .duty_cycle = 50, .duty_cycle_unit =
          TIMER_PWM_UNIT_RAW_COUNTS,
  .channel = 0, .autostart = true, .p_callback = SysTickIntHandler, .p_context = &g_timer0,
  .p_extend = &g_timer0_extend, .irq_ipl = (1), };
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{ .p_ctrl = &g_timer0_ctrl, .p_cfg = &g_timer0_cfg, .p_api = &g_timer_on_gpt };
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;
void g_hal_init(void);

void user_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */

    tx_thread_create (&user_thread, (CHAR *) "User Thread", user_thread_func, (ULONG) NULL, &user_thread_stack, 16384,
                      1, 1, 1, TX_AUTO_START);
}

static void user_thread_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* First thread will take care of common initialization. */
    UINT err;
    err = tx_semaphore_get (&g_ssp_common_initialized_semaphore, TX_WAIT_FOREVER);

    while (TX_SUCCESS != err)
    {
        /* Check err, problem occurred. */
        BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
    }

    /* Only perform common initialization if this is the first thread to execute. */
    if (false == g_ssp_common_initialized)
    {
        /* Later threads will not run this code. */
        g_ssp_common_initialized = true;

        /* Perform common module initialization. */
        g_hal_init ();

        /* Now that common initialization is done, let other threads through. */
        /* First decrement by 1 since 1 thread has already come through. */
        g_ssp_common_thread_count--;
        while (g_ssp_common_thread_count > 0)
        {
            err = tx_semaphore_put (&g_ssp_common_initialized_semaphore);

            while (TX_SUCCESS != err)
            {
                /* Check err, problem occurred. */
                BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
            }

            g_ssp_common_thread_count--;
        }
    }

    /* Initialize each module instance. */
    UINT g_dns0_err;

    /* Create DNS Client. */
    g_dns0_err = nx_dns_create (&g_dns0, &g_ip0, (UCHAR *) "g_dns0 DNS Client");

    if (NX_SUCCESS != g_dns0_err)
    {
        g_dns0_err_callback ((void *) &g_dns0, &g_dns0_err);
    }
    /** Call sf comms initialization function. User Can call at later time as well if required. */
#if (1)
    sf_comms_init0 ();
#endif
    UINT g_dhcp_client0_err;
    /* Create DHCP client. */
    g_dhcp_client0_err = nx_dhcp_create (&g_dhcp_client0, &g_ip0, "g_dhcp_client0 DHCP");
    if (NX_SUCCESS != g_dhcp_client0_err)
    {
        g_dhcp_client0_err_callback ((void *) &g_dhcp_client0, &g_dhcp_client0_err);
    }

    /* Enter user code for this thread. */
    user_thread_entry ();
}
