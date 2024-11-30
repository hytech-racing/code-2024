import { defineConfig } from 'vite';
import { viteStaticCopy } from 'vite-plugin-static-copy';

// https://vitejs.dev/config/
export default defineConfig(async () => ({
	// Vite options tailored for Tauri development and only applied in `tauri dev` or `tauri build`
	//
	// 1. prevent vite from obscuring rust errors
	clearScreen: false,
	envPrefix: ['VITE_', 'TAURI_'],
	// 2. tauri expects a fixed port, fail if that port is not available
	server: {
		port: 1420,
		strictPort: true,
		watch: {
			// 3. tell vite to ignore watching `src-tauri`
			ignored: ['**/src-tauri/**'],
		},
	},
	build: {
		cssMinify: true,

	},
	plugins: [
		viteStaticCopy({
			targets: [
				{
					src: [
						'./node_modules/openmct/dist/favicons/*',
						'./node_modules/openmct/dist/imagery/*',
						'./node_modules/openmct/dist/images/*',
					],
					dest: 'images',
				},
				{
					src: './node_modules/openmct/dist/fonts/*',
					dest: 'fonts',
				},
				{
					src: './node_modules/openmct/dist/*.css',
					dest: '',
				},
			],
		}),
	],
}));
