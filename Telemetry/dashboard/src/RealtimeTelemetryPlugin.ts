import Mqtt from './Mqtt';
import { OpenMCT } from './main';
import { DomainObject } from 'openmct/dist/src/api/objects/ObjectAPI';

/**
 * Basic Realtime telemetry plugin using websockets.
 */
export const RealtimeTelemetryPlugin = () => {
	return async (openmct: OpenMCT) => {
		const client = await Mqtt.connect(
			import.meta.env.TAURI_DEBUG
				? 'mqtt://localhost:1883'
				: 'mqtt://3.134.2.166:1883'
		);
		const listeners: Record<string, (...args: any[]) => any> = {};

		client.onMessage(({ topic, payload }) => {
			const channel = topic.replace(/\//g, '.');

			listeners[channel]?.({
				timestamp: new Date().getTime(),
				value: parseFloat(payload),
				id: Date.now(),
			});
		});

		const provider = {
			supportsSubscribe: (domainObject: DomainObject) => {
				return domainObject.type === 'dashboard.telemetry';
			},
			subscribe: (
				domainObject: DomainObject,
				callback: (...args: any[]) => any
			) => {
				listeners[domainObject.identifier.key] = callback;
				const topic = domainObject.identifier.key.replace(/\./g, '/');
				client.subscribe(topic);

				return () => {
					client.unsubscribe(topic);
					delete listeners[domainObject.identifier.key];
				};
			},
		};

		openmct.telemetry.addProvider(provider);
	};
};
