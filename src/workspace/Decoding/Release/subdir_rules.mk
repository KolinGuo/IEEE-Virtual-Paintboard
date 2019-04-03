################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
gpio_if.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/gpio_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib/" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc/" --define=ccs --define=NON_NETWORK --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="gpio_if.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib/" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc/" --define=ccs --define=NON_NETWORK --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

pin_mux_config.obj: ../pin_mux_config.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib/" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc/" --define=ccs --define=NON_NETWORK --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="pin_mux_config.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_ccs.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/startup_ccs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib/" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc/" --define=ccs --define=NON_NETWORK --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="startup_ccs.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

timer_if.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/timer_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib/" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc/" --define=ccs --define=NON_NETWORK --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="timer_if.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

uart_if.obj: ../uart_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/driverlib/" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/inc/" --define=ccs --define=NON_NETWORK --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="uart_if.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


