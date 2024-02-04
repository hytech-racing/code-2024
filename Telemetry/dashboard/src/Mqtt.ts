import { invoke, transformCallback } from '@tauri-apps/api/tauri';

export interface ConnectionConfig {
	retry: boolean;
}

interface MessageKind<T, D> {
	type: T;
	data: D;
}

type Event =
	| MessageKind<'message', { topic: string; payload: string }>
	| MessageKind<'error', string>
	| MessageKind<'connect', null>;

export default class Mqtt {
	private constructor(
		public readonly id: number,
		private readonly messageListeners: Array<
			(message: { topic: string; payload: string }) => void
		>,
		private readonly errorListeners: Array<(err: string) => void>,
		private readonly connectListeners: Array<() => void>
	) {}

	static async connect(url: string, config?: ConnectionConfig): Promise<Mqtt> {
		const messageListeners: Array<
			(message: { topic: string; payload: string }) => void
		> = [];
		const errorListeners: Array<(err: string) => void> = [];

		let notifyConnect: () => void;
		const connected = new Promise<void>((res) => (notifyConnect = res));
		const connectListeners: Array<() => void> = [() => notifyConnect()];

		const handler = (e: Event): void => {
			console.log(e);
			if (e.type === 'connect') {
				connectListeners.forEach((l) => l());
			} else if (e.type === 'error') {
				errorListeners.forEach((l) => l(e.data));
			} else if (e.type === 'message') {
				messageListeners.forEach((l) => l(e.data));
			}
		};

		console.log('connecting mqtt')
		const client = await invoke<number>('plugin:mqtt|connect', {
			url,
			callbackFunction: transformCallback(handler),
			config,
		}).then(
			(id) => new Mqtt(id, messageListeners, errorListeners, connectListeners)
		);

		await connected;
		console.log('connected mqtt');

		return client;
	}

	onMessage(cb: (arg: { topic: string; payload: string }) => void): void {
		this.messageListeners.push(cb);
	}

	onError(cb: (arg: string) => void): void {
		this.errorListeners.push(cb);
	}

	async subscribe(topic: string): Promise<void> {
		return await invoke('plugin:mqtt|subscribe', {
			id: this.id,
			topic,
		});
	}

	async unsubscribe(topic: string): Promise<void> {
		return await invoke('plugin:mqtt|unsubscribe', {
			id: this.id,
			topic,
		});
	}

	async publish(topic: string, message: any): Promise<void> {
		return await invoke('plugin:mqtt|publish', {
			id: this.id,
			topic,
			message: JSON.stringify(message),
		});
	}

	async disconnect(): Promise<void> {
		return await invoke('plugin:mqtt|disconnect', {
			id: this.id,
		});
	}
}
