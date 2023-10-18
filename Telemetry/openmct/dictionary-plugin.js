function getDictionary() {
    return http.get('/dictionary.json')
        .then(function (result) {
            return result.data;
        });
}

var objectProvider = {
    get: function (identifier) {
        return getDictionary().then(function (dictionary) {
            if (identifier.key === 'HT08') {
                return {
                    identifier: identifier,
                    name: dictionary.name,
                    type: 'folder',
                    location: 'ROOT'
                };
            } else {
                var measurement = dictionary.measurements.filter(function (m) {
                    return m.key === identifier.key;
                })[0];
                return {
                    identifier: identifier,
                    name: measurement.name,
                    type: 'dashboard.telemetry',
                    telemetry: {
                        values: measurement.values
                    },
                    location: 'dashboard:HT08'
                };
            }
        });
    }
};

var compositionProvider = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
               domainObject.type === 'folder';
    },
    load: function (domainObject) {
        return getDictionary()
            .then(function (dictionary) {
                return dictionary.measurements.map(function (m) {
                    return {
                        namespace: 'dashboard',
                        key: m.key
                    };
                });
            });
    }
};

var DictionaryPlugin = function (openmct) {
    return function install(openmct) {
        openmct.objects.addRoot({
            namespace: 'dashboard',
            key: 'HT08'
        });

        openmct.objects.addProvider('dashboard', objectProvider);

        openmct.composition.addProvider(compositionProvider);

        openmct.types.addType('dashboard.telemetry', {
            name: 'Data Series',
            description: 'Data coming in from a sensor on the car',
            cssClass: 'icon-telemetry'
        });
    };
};
