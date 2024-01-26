#!/usr/bin/env -S deno run --allow-read --allow-write --allow-run

// Usage: ./release.ts [type]
// type - "major" "minor" "patch"

import {
	increment,
	ReleaseType,
} from 'https://deno.land/std@0.181.0/semver/mod.ts';

type TauriConfig = {
	package: {
		version: string;
	};
};

const config: TauriConfig = JSON.parse(
	Deno.readTextFileSync('./src-tauri/tauri.conf.json')
);

const newVer = increment(config.package.version, Deno.args[0] as ReleaseType);

if (newVer === null) {
	console.error('Something went wrong updating the version, args:', Deno.args);
	Deno.exit(1);
}

const newVerWithV = 'v' + newVer;
const tag = `dashboard-${newVerWithV}`;

if (!confirm(`Do you want to publish ${newVerWithV}?`)) Deno.exit(0);

config.package.version = newVer;
Deno.writeTextFileSync(
	'./src-tauri/tauri.conf.json',
	JSON.stringify(config, null, '\t') + '\n'
);

const run = (args: string[]) => {
	const process = Deno.run({ cmd: args, stdout: 'piped' });
	return Promise.all([process.output(), process.status()]).then(() =>
		process.close()
	);
};

/*
git add .
git commit -m "[release] $1"
git push origin
git tag $1
git push origin $1
*/
await run(['git', 'add', '.']);
await run(['git', 'commit', '-m', `[release] Live telemetry ${newVerWithV}`]);
await run(['git', 'push', 'origin']);
await run(['git', 'tag', tag]);
await run(['git', 'push', 'origin', tag]);
