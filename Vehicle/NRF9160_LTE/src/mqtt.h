#ifndef MQTT_H
#define MQTT_H
#include <stdio.h>
#include <zephyr/net/mqtt.h>

static struct mqtt_client client;

int data_publish(struct mqtt_client *c, enum mqtt_qos qos, uint8_t *data, size_t len);

void mqtt_thread(void);
#endif