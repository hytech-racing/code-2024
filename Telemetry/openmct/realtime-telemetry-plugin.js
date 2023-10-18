/**
 * Basic Realtime telemetry plugin using websockets.
 */
function RealtimeTelemetryPlugin() {
    return function (openmct) {

        var client = new Paho.MQTT.Client('localhost', 80, "/", "unique_ID");
        var globalChannels = [];
        myClientConnected = function () {
            console.log("connected");
            //console.log("Channel: "+ (domainObject.identifier.key).replaceAll(".", "/"));
            for (let i = 0; i < globalChannels.length; i++) {
                client.subscribe(globalChannels[i], 0);
            }
            //client.subscribe("MOTOR_CONTROLLER/mc_rl/torque_current", 0);
        }



        //var socket = new WebSocket(location.origin.replace(/^http/, 'ws') + '/realtime/');
        var listener = {};

        client.onMessageArrived = function (message) {
            channel = (message.destinationName.replaceAll("/", "."));
            if (listener[channel]) {
                listener[channel]({
                    timestamp: new Date().getTime(),
                    value: parseFloat(message.payloadString),
                    id: Date.now(),
                });
            }
        };

        var provider = {
            supportsSubscribe: function (domainObject) {
                return domainObject.type === 'dashboard.telemetry';
            },
            subscribe: function (domainObject, callback) {
                listener[domainObject.identifier.key] = callback;
                globalChannels.push((domainObject.identifier.key).replaceAll(".", "/"));
                if (client.isConnected()) {
                    client.disconnect();
                }
                client.connect({onSuccess: myClientConnected});

                return function unsubscribe() {
                    delete listener[(domainObject.identifier.key)];
                    //client.unsubscribe(domainObject.identifier.key);
                    client.disconnect();
                };
            }
        };

        openmct.telemetry.addProvider(provider);

        /*
        var socket = new WebSocket(location.origin.replace(/^http/, 'ws') + '/realtime/');
        var listener = {};

        socket.onmessage = function (event) {
            point = JSON.parse(event.data);
            if (listener[point.id]) {
                listener[point.id](point);
            }
        };

        var provider = {
            supportsSubscribe: function (domainObject) {
                return domainObject.type === 'dashboard.telemetry';
            },
            subscribe: function (domainObject, callback) {
                listener[domainObject.identifier.key] = callback;
                socket.send('subscribe ' + domainObject.identifier.key);
                return function unsubscribe() {
                    delete listener[domainObject.identifier.key];
                    socket.send('unsubscribe ' + domainObject.identifier.key);
                };
            }
        };

        openmct.telemetry.addProvider(provider);

         */

        /*

         */
    }


}
