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
            }

            else if (identifier.key === "MOTOR_CONTROLLER") {
                var subsystem = dictionary.subsystems.filter(function (m) {
                    return m.key === identifier.key;
                })[0];
                return {
                    identifier: identifier,
                    name: subsystem.name,
                    type: 'folder',
                    location: 'dashboard:HT08'
                };
            }
            else {
                    subsystem = dictionary.subsystems.filter(function (m) {
                        return m.key === identifier.key.split(".")[0];
                    })[0];
                    var measurement = subsystem.measurements.filter(function (m) {
                        return m.key === identifier.key;
                    })[0];
                    console.log("Added" + identifier.key)
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
               domainObject.type === 'folder'
                && domainObject.identifier.key === 'HT08';
    },
    load: function (domainObject) {

        return getDictionary()
            .then(function (dictionary) {
                return dictionary.subsystems.map(function (s) {
                        return {
                            namespace: 'dashboard',
                            key: s.key
                        }
                    });


            });

    }
};

var compositionProviderMC = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'MOTOR_CONTROLLER';
    },
    load: function (domainObject) {
        return [{namespace: 'dashboard', key: 'MOTOR_CONTROLLER.mc_rl.torque_current'},
                {namespace: 'dashboard', key: 'MOTOR_CONTROLLER.mc_rl.magnetizing_current'}]

        /* does not work for some stupid reason
        return getDictionary()
            .then(function (dictionary) {
                return dictionary.subsystems.map(function (s) {
                    if (s.key === "MOTOR_CONTROLLER") {
                        return s.measurements.map(function (m) {
                            console.log(m.key);
                            return {
                                namespace: 'dashboard',
                                key: m.key
                            }

                        });
                    }
                });

            });
           */
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
        openmct.composition.addProvider(compositionProviderMC);

        openmct.types.addType('dashboard.telemetry', {
            name: 'Data Series',
            description: 'Data coming in from a sensor on the car',
            cssClass: 'icon-telemetry'
        });
    };
};
