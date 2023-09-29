//test script for MQTT in JavaScript, most of this code was moved to Test.html

var client = new Paho.MQTT.Client('localhost', 8080, "/", "iawgewagwd");

client.connect({ onSuccess: myClientConnected });


// This is the function which handles subscribing to topics after a connection is made
function myClientConnected() {
    $("#updateMe").prepend("Connected");
    client.subscribe("MOTOR_CONTROLLER/mc_rr/motor_temp");
}

// This is the function which handles received messages
function myMessageArrived(message) {
    // Get the payload
    var messageBody = message.payloadString;

    // Create a new HTML element wrapping the message payload
    var messageHTML = $("<p>"+messageBody+"</p>");

    // Insert it inside the ```id=updateMe``` element above everything else that is there
    $("#updateMe").prepend(messageHTML);
};

// Tell MQTT_CLIENT to call myMessageArrived(message) each time a new message arrives
client.onMessageArrived = myMessageArrived;