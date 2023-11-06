function getDictionary() {
    return http.get('/dictionary.json')
        .then(function (result) {
            return result.data;
        });
}
folders = ["BMS", "DASHBOARD", "MCU", "mc_fl", "mc_fr", "mc_rl", "mc_rr", "ENERGY_METER", "IMU", "TPMS"]
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

            else if (folders.includes(identifier.key)) {
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
                        if (identifier.key.split(".")[0] === "MOTOR_CONTROLLER") {
                            return m.key === identifier.key.split(".")[1];
                        }
                        return m.key === identifier.key.split(".")[0];
                    })[0];
                    var measurement = subsystem.measurements.filter(function (m) {
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
        getDictionary()
            .then(function (dictionary) {
                 dictionary.subsystems.map(function (s) {
                    if (s.key === "MOTOR_CONTROLLER") {
                        s.measurements.map(function (m) {
                            console.log(m.key);
                            objects.push( {
                                namespace: 'dashboard',
                                key: m.key
                            });

                        });
                    }
                });
                console.log(objects);
                return objects;
            });

         */


    }
};

var compositionProviderBMS = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'BMS';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'BMS.detailed_temps.ic'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_temps.group'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_temps.temps[0]'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_temps.temps[1]'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_temps.temps[2]'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_voltage.ic'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_voltage.group'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_voltage.voltage[0]'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_voltage.voltage[1]'}, {'namespace': 'dashboard', 'key': 'BMS.detailed_voltage.voltage[2]'}, {'namespace': 'dashboard', 'key': 'BMS.onboard_temperatures.average_temperature'}, {'namespace': 'dashboard', 'key': 'BMS.onboard_temperatures.low_temperature'}, {'namespace': 'dashboard', 'key': 'BMS.onboard_temperatures.high_temperature'}, {'namespace': 'dashboard', 'key': 'BMS.temperatures.average_temperature'}, {'namespace': 'dashboard', 'key': 'BMS.temperatures.low_temperature'}, {'namespace': 'dashboard', 'key': 'BMS.temperatures.high_temperature'}, {'namespace': 'dashboard', 'key': 'BMS.voltages.average_voltage'}, {'namespace': 'dashboard', 'key': 'BMS.voltages.low_voltage'}, {'namespace': 'dashboard', 'key': 'BMS.voltages.high_voltage'}, {'namespace': 'dashboard', 'key': 'BMS.voltages.total_voltage'}]
    }
};

var compositionProviderDASHBOARD = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'DASHBOARD';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'DASHBOARD.start_btn'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.buzzer_active'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.ssok_above_threshold'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.shutdown_h_above_threshold'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.mark_btn'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.mode_btn'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.mc_cycle_btn'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.launch_ctrl_btn'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.torque_mode_btn'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.led_dimmer_btn'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.dial_state'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.ams_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.imd_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.mode_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.mc_error_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.start_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.inertia_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.mech_brake_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.gen_purp_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.bots_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.cockpit_brb_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.crit_charge_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.glv_led'}, {'namespace': 'dashboard', 'key': 'DASHBOARD.launch_control_led'}]
    }
};

var compositionProviderMCU = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'MCU';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'MCU.GPS.latitude'}, {'namespace': 'dashboard', 'key': 'MCU.GPS.longitude'}, {'namespace': 'dashboard', 'key': 'MCU.GPS.msl'}, {'namespace': 'dashboard', 'key': 'MCU.GPS.accuracy'}, {'namespace': 'dashboard', 'key': 'MCU.pedals.accelerator_pedal_1'}, {'namespace': 'dashboard', 'key': 'MCU.pedals.accelerator_pedal_2'}, {'namespace': 'dashboard', 'key': 'MCU.pedals.brake_pedal_1'}, {'namespace': 'dashboard', 'key': 'MCU.pedals.brake_pedal_2'}, {'namespace': 'dashboard', 'key': 'MCU.status.shutdown.imd_ok_high'}, {'namespace': 'dashboard', 'key': 'MCU.status.shutdown.shutdown_b_above_threshold'}, {'namespace': 'dashboard', 'key': 'MCU.status.shutdown.bms_ok_high'}, {'namespace': 'dashboard', 'key': 'MCU.status.shutdown.shutdown_c_above_threshold'}, {'namespace': 'dashboard', 'key': 'MCU.status.shutdown.bspd_ok_high'}, {'namespace': 'dashboard', 'key': 'MCU.status.shutdown.shutdown_d_above_threshold'}, {'namespace': 'dashboard', 'key': 'MCU.status.shutdown.software_ok_high'}, {'namespace': 'dashboard', 'key': 'MCU.status.shutdown.shutdown_e_above_threshold'}, {'namespace': 'dashboard', 'key': 'MCU.status.pedals.mech_brake_active'}, {'namespace': 'dashboard', 'key': 'MCU.status.pedals.no_accel_implausability'}, {'namespace': 'dashboard', 'key': 'MCU.status.pedals.no_brake_implausability'}, {'namespace': 'dashboard', 'key': 'MCU.status.pedals.brake_pedal_active'}, {'namespace': 'dashboard', 'key': 'MCU.status.pedals.bspd_current_high'}, {'namespace': 'dashboard', 'key': 'MCU.status.pedals.bspd_brake_high'}, {'namespace': 'dashboard', 'key': 'MCU.status.pedals.no_accel_brake_implausability'}, {'namespace': 'dashboard', 'key': 'MCU.status.ecu.mcu_state'}, {'namespace': 'dashboard', 'key': 'MCU.status.ecu.inverter_powered'}, {'namespace': 'dashboard', 'key': 'MCU.status.ecu.energy_meter_present'}, {'namespace': 'dashboard', 'key': 'MCU.status.ecu.activate_buzzer'}, {'namespace': 'dashboard', 'key': 'MCU.status.ecu.software_is_ok'}, {'namespace': 'dashboard', 'key': 'MCU.status.ecu.launch_ctrl_active'}, {'namespace': 'dashboard', 'key': 'MCU.status.ecu.pack_charge_critical'}, {'namespace': 'dashboard', 'key': 'MCU.status.max_torque'}, {'namespace': 'dashboard', 'key': 'MCU.status.torque_mode'}, {'namespace': 'dashboard', 'key': 'MCU.status.distance_travelled'}, {'namespace': 'dashboard', 'key': 'MCU.analog.steering_1'}, {'namespace': 'dashboard', 'key': 'MCU.analog.steering_2'}, {'namespace': 'dashboard', 'key': 'MCU.analog.hall_effect'}, {'namespace': 'dashboard', 'key': 'MCU.analog.glv_battery_voltage'}, {'namespace': 'dashboard', 'key': 'MCU.load_cells.FL'}, {'namespace': 'dashboard', 'key': 'MCU.load_cells.FR'}, {'namespace': 'dashboard', 'key': 'MCU.load_cells.RL'}, {'namespace': 'dashboard', 'key': 'MCU.load_cells.RR'}, {'namespace': 'dashboard', 'key': 'MCU.pots.pot1'}, {'namespace': 'dashboard', 'key': 'MCU.pots.pot2'}, {'namespace': 'dashboard', 'key': 'MCU.pots.pot3'}, {'namespace': 'dashboard', 'key': 'MCU.pots.pot4'}, {'namespace': 'dashboard', 'key': 'MCU.pots.pot5'}, {'namespace': 'dashboard', 'key': 'MCU.pots.pot6'}]
    }
};

var compositionProviderMC_FL = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'mc_fl';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.dc_bus_voltage'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.actual_power'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.feedback_torque'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.inverter_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.hv_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.driver_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.remove_error'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.speed_setpoint'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.pos_torque_limit'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.neg_torque_limit'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.system_ready'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.error'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.warning'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.quit_dc_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.dc_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.quit_inverter_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.inverter_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.derating_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.speed'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.torque_current'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.magnetizing_current'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.motor_temp'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.inverter_temp'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.diagnostic_number'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fl.igbt_temp'}]
    }
};

var compositionProviderMC_FR = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'mc_fr';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.dc_bus_voltage'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.actual_power'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.feedback_torque'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.inverter_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.hv_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.driver_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.remove_error'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.speed_setpoint'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.pos_torque_limit'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.neg_torque_limit'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.system_ready'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.error'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.warning'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.quit_dc_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.dc_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.quit_inverter_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.inverter_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.derating_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.speed'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.torque_current'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.magnetizing_current'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.motor_temp'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.inverter_temp'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.diagnostic_number'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_fr.igbt_temp'}]
    }
};

var compositionProviderMC_RL = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'mc_rl';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.dc_bus_voltage'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.actual_power'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.feedback_torque'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.inverter_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.hv_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.driver_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.remove_error'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.speed_setpoint'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.pos_torque_limit'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.neg_torque_limit'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.system_ready'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.error'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.warning'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.quit_dc_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.dc_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.quit_inverter_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.inverter_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.derating_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.speed'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.torque_current'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.magnetizing_current'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.motor_temp'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.inverter_temp'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.diagnostic_number'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rl.igbt_temp'}]
    }
};

var compositionProviderMC_RR = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'mc_rr';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.dc_bus_voltage'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.actual_power'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.feedback_torque'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.inverter_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.hv_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.driver_enable'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.remove_error'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.speed_setpoint'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.pos_torque_limit'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.neg_torque_limit'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.system_ready'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.error'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.warning'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.quit_dc_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.dc_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.quit_inverter_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.inverter_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.derating_on'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.speed'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.torque_current'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.magnetizing_current'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.motor_temp'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.inverter_temp'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.diagnostic_number'}, {'namespace': 'dashboard', 'key': 'MOTOR_CONTROLLER.mc_rr.igbt_temp'}]
    }
};

var compositionProviderENERGY_METER = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'ENERGY_METER';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'ENERGY_METER.voltage_gain'}, {'namespace': 'dashboard', 'key': 'ENERGY_METER.current_gain'}, {'namespace': 'dashboard', 'key': 'ENERGY_METER.overvoltage'}, {'namespace': 'dashboard', 'key': 'ENERGY_METER.overpower'}, {'namespace': 'dashboard', 'key': 'ENERGY_METER.logging'}, {'namespace': 'dashboard', 'key': 'ENERGY_METER.voltage'}, {'namespace': 'dashboard', 'key': 'ENERGY_METER.current'}]
    }
};

var compositionProviderIMU = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'IMU';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'IMU.lat_accel'}, {'namespace': 'dashboard', 'key': 'IMU.long_accel'}, {'namespace': 'dashboard', 'key': 'IMU.vert_accel'}, {'namespace': 'dashboard', 'key': 'IMU.yaw'}, {'namespace': 'dashboard', 'key': 'IMU.pitch'}, {'namespace': 'dashboard', 'key': 'IMU.roll'}]
    }
};

var compositionProviderTPMS = {
    appliesTo: function (domainObject) {
        return domainObject.identifier.namespace === 'dashboard' &&
            domainObject.type === 'folder'
            && domainObject.identifier.key === 'TPMS';
    },
    load: function (domainObject) {
        return [{'namespace': 'dashboard', 'key': 'TPMS.LF.guage_pressure'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.hs_pressure'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.batt_voltage'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.serial_no'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_4'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_3'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_2'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_1'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_8'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_7'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_6'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_5'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_12'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_11'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_10'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_9'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_16'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_15'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_14'}, {'namespace': 'dashboard', 'key': 'TPMS.LF.temp_13'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.guage_pressure'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.hs_pressure'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.batt_voltage'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.serial_no'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_4'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_3'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_2'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_1'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_8'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_7'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_6'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_5'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_12'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_11'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_10'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_9'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_16'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_15'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_14'}, {'namespace': 'dashboard', 'key': 'TPMS.RF.temp_13'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.guage_pressure'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.hs_pressure'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.batt_voltage'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.serial_no'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_4'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_3'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_2'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_1'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_8'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_7'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_6'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_5'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_12'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_11'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_10'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_9'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_16'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_15'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_14'}, {'namespace': 'dashboard', 'key': 'TPMS.LR.temp_13'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.guage_pressure'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.hs_pressure'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.batt_voltage'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.serial_no'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_4'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_3'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_2'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_1'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_8'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_7'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_6'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_5'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_12'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_11'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_10'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_9'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_16'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_15'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_14'}, {'namespace': 'dashboard', 'key': 'TPMS.RR.temp_13'}]
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
        openmct.composition.addProvider(compositionProviderBMS);
        openmct.composition.addProvider(compositionProviderDASHBOARD);
        openmct.composition.addProvider(compositionProviderMCU);
        openmct.composition.addProvider(compositionProviderMC_FL);
        openmct.composition.addProvider(compositionProviderMC_FR);
        openmct.composition.addProvider(compositionProviderMC_RL);
        openmct.composition.addProvider(compositionProviderMC_RR);
        openmct.composition.addProvider(compositionProviderENERGY_METER);
        openmct.composition.addProvider(compositionProviderIMU);
        openmct.composition.addProvider(compositionProviderTPMS);

        openmct.types.addType('dashboard.telemetry', {
            name: 'Data Series',
            description: 'Data coming in from a sensor on the car',
            cssClass: 'icon-telemetry'
        });
    };
};
