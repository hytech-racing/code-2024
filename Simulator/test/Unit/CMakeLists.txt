set(BINARY unit_tests)
add_executable	(unit_tests	test_ADC.cpp 
							test_ADXL.cpp
							test_GPS.cpp
							test_HTCAN
							test_LTC.cpp
							test_MCP.cpp
				)
target_link_libraries	(unit_tests PUBLIC 
							GTEST
							CORE 
							ADC
							ACCEL
							CAN
							EXPANDER
							GPS
							HYTECH_CAN
							LTC
						)