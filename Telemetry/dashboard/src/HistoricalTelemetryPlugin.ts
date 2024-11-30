import { DomainObject } from 'openmct/dist/src/api/objects/ObjectAPI';
import { OpenMCT } from './main';

/**
 * Basic historical telemetry plugin.
 */
export const HistoricalTelemetryPlugin = () => {
	return (openmct: OpenMCT) => {
		var provider = {
			supportsRequest: (domainObject: DomainObject) => {
				return domainObject.type === 'dashboard.telemetry';
			},
			request: (domainObject: DomainObject, options: any) => {
				return Promise.resolve([]);
				/*
                var url = '/history/' +
                    domainObject.identifier.key +
                    '?start=' + options.start +
                    '&end=' + options.end;

                return http.get(url)
                    .then(function (resp) {
                        return resp.data;
                    });
                    */
			},
		};

		openmct.telemetry.addProvider(provider);
	};
};
