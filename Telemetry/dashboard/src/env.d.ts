/// <reference types="vite/client" />

interface ImportMetaEnv {
	readonly VITE_APP_TITLE: string;
	readonly TAURI_PLATFORM: string;
	readonly TAURI_ARCH: string;
	readonly TAURI_FAMILY: string;
	readonly TAURI_PLATFORM_VERSION: string;
	readonly TAURI_PLATFORM_TYPE: string;
	readonly TAURI_DEBUG: string;
}

interface ImportMeta {
	readonly env: ImportMetaEnv;
}
