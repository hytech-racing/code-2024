/**
 * Basic historical telemetry plugin.
 */

function HistoricalTelemetryPlugin() {
    return function install (openmct) {
        var provider = {
            supportsRequest: function (domainObject) {
                return domainObject.type === 'dashboard.telemetry';
            },
            request: function (domainObject, options) {
                return Promise.resolve([])
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

            }
        };

        openmct.telemetry.addProvider(provider);
    }
}
