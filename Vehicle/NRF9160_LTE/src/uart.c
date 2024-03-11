#include <zephyr/zephyr.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <drivers/gpio.h>
#include <zephyr/net/mqtt.h>
#include <string.h>
#include "mqtt.c"

//LOG_MODULE_DECLARE(mqtt_simple, CONFIG_MQTT_SIMPLE_LOG_LEVEL);

/* 
 * Serial Stuff
 */
/* change this to any other UART peripheral if desired */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
//#define UART_DEVICE_NODE DT_NODELABEL(uart1)

#define MSG_SIZE 64

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static const struct device *gpio = DEVICE_DT_GET(DT_NODELABEL(gpio0));

/* receive buffer used in UART ISR callback */
static char rx_buf[MSG_SIZE];
static int rx_buf_pos;
extern struct mqtt_client client;

/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	while (uart_irq_rx_ready(uart_dev)) {

		uart_fifo_read(uart_dev, &c, 1);

		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			//data_publish(&client, MQTT_QOS_1_AT_LEAST_ONCE,
			//	rx_buf, strlen(rx_buf) + 1);

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

/*
 * Print a null-terminated string character by character to the UART interface
 */
void print_uart(char *buf)
{
	int msg_len = strlen(buf);
	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart_dev, buf[i]);
	}
}

void uart_thread(void) {
	const struct uart_config cfg = {
		.baudrate = 115200, 
		.parity = UART_CFG_PARITY_NONE, 
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
		};
	uart_configure(uart_dev, &cfg);
	char tx_buf[MSG_SIZE];

	if (!device_is_ready(uart_dev)) {
		LOG_INF("UART device not found!");
		return;
	}

	if (!device_is_ready(gpio)) {
		LOG_INF("GPIO device not found!");
		return;
	}


	/* configure interrupt and callback to receive data */
	uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);
	uart_irq_rx_enable(uart_dev);

	LOG_INF("UART device ready");

	//uint8_t c;

	while (1) {
		if (k_msgq_get(&uart_msgq, &tx_buf, K_FOREVER) == 0) {
			print_uart("Echo: ");
			print_uart(tx_buf);
			print_uart("\r\n");
			
			int ret;

			ret = data_publish(&client,
					MQTT_QOS_1_AT_LEAST_ONCE,
					CONFIG_BUTTON_EVENT_PUBLISH_MSG,
					sizeof(CONFIG_BUTTON_EVENT_PUBLISH_MSG));
			
			ret = data_publish(&client,
					MQTT_QOS_1_AT_LEAST_ONCE,
					tx_buf,
					strlen(tx_buf)+1);
			if (ret) {
				LOG_ERR("Publish failed: %d", ret);
			}

		}
		/*
		if (!uart_poll_in(uart_dev, &c))
		{
			if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
				// terminate string
				rx_buf[rx_buf_pos] = '\0';
				//data_publish(&client, MQTT_QOS_1_AT_LEAST_ONCE,
				//	rx_buf, strlen(rx_buf) + 1);
				// if queue is full, message is silently dropped
				k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);
				// reset the buffer (it was copied to the msgq)
				rx_buf_pos = 0;
			} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
				rx_buf[rx_buf_pos++] = c;
			}
			// else: characters beyond buffer size are dropped
		}
		*/
	}


}
