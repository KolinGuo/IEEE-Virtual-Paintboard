################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SparkFun_VL53L1X.obj: ../SparkFun_VL53L1X.cpp $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=1 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc" --define=ccs --define=NON_NETWORK --define=cc3200 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="SparkFun_VL53L1X.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

i2c_if.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/i2c_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=1 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc" --define=ccs --define=NON_NETWORK --define=cc3200 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="i2c_if.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.cpp $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=1 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc" --define=ccs --define=NON_NETWORK --define=cc3200 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

pin_mux_config.obj: ../pin_mux_config.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=1 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc" --define=ccs --define=NON_NETWORK --define=cc3200 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="pin_mux_config.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_ccs.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/startup_ccs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=1 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc" --define=ccs --define=NON_NETWORK --define=cc3200 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="startup_ccs.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

uart_if.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/uart_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=1 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc" --define=ccs --define=NON_NETWORK --define=cc3200 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="uart_if.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

vl53l1x_class.obj: ../vl53l1x_class.cpp $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=1 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc" --define=ccs --define=NON_NETWORK --define=cc3200 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="vl53l1x_class.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


