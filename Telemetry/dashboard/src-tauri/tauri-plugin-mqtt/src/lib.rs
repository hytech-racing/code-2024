use std::{collections::HashMap, time::Duration};

use rumqttc::{AsyncClient, ConnectReturnCode, ConnectionError, Event, Incoming, MqttOptions};
use serde::{Deserialize, Serialize, Serializer};
use tauri::{
    api::ipc::{format_callback, CallbackFn},
    http::Uri,
    plugin::{Builder, TauriPlugin},
    Manager, Runtime, State, Window,
};
use tokio::sync::Mutex;

type Id = u32;
type Result<T> = std::result::Result<T, Error>;

#[derive(Debug, thiserror::Error)]
enum Error {
    #[error(transparent)]
    Mqtt(#[from] rumqttc::Error),
    #[error(transparent)]
    Client(#[from] rumqttc::ClientError),
    #[error("Passed in an invalid url: {0:?}")]
    MalformedUrl(String),
    #[error("Couldn't find a client with id: {0}")]
    ConnectionNotFound(Id),
}

impl Serialize for Error {
    fn serialize<S>(&self, serializer: S) -> std::result::Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        serializer.serialize_str(self.to_string().as_str())
    }
}

#[derive(Default)]
struct ConnectionManager(Mutex<HashMap<Id, AsyncClient>>);

#[derive(Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ConnectionConfig {}

#[derive(Deserialize, Serialize)]
#[serde(rename_all = "camelCase", tag = "type", content = "data")]
enum Message {
    Message { topic: String, payload: String },
    Error(String),
    Connect,
}

fn call_callback<R: Runtime>(
    window: Window<R>,
    callback_function: CallbackFn,
    argument: &impl Serialize,
) {
    let js =
        format_callback(callback_function, argument).expect("unable to serialize mqtt message");
    let _ = window.eval(js.as_str());
}

#[tauri::command]
async fn connect<R: Runtime>(
    window: Window<R>,
    url: String,
    callback_function: CallbackFn,
    config: Option<ConnectionConfig>,
) -> Result<Id> {
    let id: Id = rand::random();

    let uri: Uri = url.parse().map_err(|_| Error::MalformedUrl(url.clone()))?;

    let options = MqttOptions::new(
        id.to_string(),
        uri.host().ok_or_else(|| Error::MalformedUrl(url.clone()))?,
        uri.port_u16()
            .ok_or_else(|| Error::MalformedUrl(url.clone()))?,
    );

    let (client, mut event_loop) = AsyncClient::new(options, 64);
    let manager = window.state::<ConnectionManager>();
    manager.0.lock().await.insert(id, client);

    tauri::async_runtime::spawn({
        let window = window.clone();
        async move {
            let manager = window.state::<ConnectionManager>();
            loop {
                match event_loop.poll().await {
                    Ok(event) => match event {
                        Event::Incoming(incoming) => match incoming {
                            Incoming::ConnAck(conn_ack) => match conn_ack.code {
                                ConnectReturnCode::Success => {
                                    println!("connected");
                                    call_callback(
                                        window.clone(),
                                        callback_function,
                                        &Message::Connect,
                                    );
                                }
                                _ => {
                                    manager.0.lock().await.remove(&id);
                                    call_callback(
                                        window.clone(),
                                        callback_function,
                                        &Message::Error(format!(
                                            "Failed to connect to broker: {:?}",
                                            conn_ack
                                        )),
                                    );
                                    break;
                                }
                            },
                            Incoming::Publish(publish) => {
                                let topic = publish.topic;
                                let Ok(payload) = String::from_utf8(publish.payload.to_vec())
                                else {
                                    println!("Received non-utf8 message from broker. Id: {id}; Topic: {topic}");
                                    continue;
                                };

                                call_callback(
                                    window.clone(),
                                    callback_function,
                                    &Message::Message { topic, payload },
                                );
                            }
                            _ => {}
                        },
                        _ => {}
                    },
                    Err(err) => match err {
                        ConnectionError::ConnectionRefused(_) | ConnectionError::Io(_) => {
                            // Keep running to retry connection
                            tokio::time::sleep(Duration::from_secs(3)).await;
                        }
                        _ => {
                            // Tell JS about error and close client
                            manager.0.lock().await.remove(&id);
                            call_callback(
                                window.clone(),
                                callback_function,
                                &Message::Error(format!("Error in connection to broker: {err:?}")),
                            );
                            break;
                        }
                    },
                }
            }
        }
    });

    Ok(id)
}

#[tauri::command]
async fn subscribe(manager: State<'_, ConnectionManager>, id: Id, topic: String) -> Result<()> {
    if let Some(client) = manager.0.lock().await.get_mut(&id) {
        client.subscribe(topic.clone(), rumqttc::QoS::AtLeastOnce).await?;
        println!("subscribed to: {topic}");
        Ok(())
    } else {
        Err(Error::ConnectionNotFound(id))
    }
}

#[tauri::command]
async fn unsubscribe(manager: State<'_, ConnectionManager>, id: Id, topic: String) -> Result<()> {
    if let Some(client) = manager.0.lock().await.get_mut(&id) {
        client.unsubscribe(topic).await?;
        Ok(())
    } else {
        Err(Error::ConnectionNotFound(id))
    }
}

#[tauri::command]
async fn disconnect(manager: State<'_, ConnectionManager>, id: Id) -> Result<()> {
    if let Some(client) = manager.0.lock().await.get_mut(&id) {
        client.disconnect().await?;
    }

    manager.0.lock().await.remove(&id);
    Ok(())
}

#[tauri::command]
async fn publish(
    manager: State<'_, ConnectionManager>,
    id: Id,
    topic: String,
    message: String,
) -> Result<()> {
    if let Some(client) = manager.0.lock().await.get_mut(&id) {
        client
            .publish(topic, rumqttc::QoS::AtLeastOnce, false, message)
            .await?;
        Ok(())
    } else {
        Err(Error::ConnectionNotFound(id))
    }
}

pub fn init<R: Runtime>() -> TauriPlugin<R> {
    Builder::new("mqtt")
        .invoke_handler(tauri::generate_handler![
            connect,
            subscribe,
            publish,
            unsubscribe,
            disconnect
        ])
        .setup(|app| {
            app.manage(ConnectionManager::default());
            Ok(())
        })
        .build()
}
