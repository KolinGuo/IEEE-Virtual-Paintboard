################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Adafruit_GFX.obj: ../Adafruit_GFX.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="Adafruit_GFX.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Adafruit_OLED.obj: ../Adafruit_OLED.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="Adafruit_OLED.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

camera_app.obj: ../camera_app.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="camera_app.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

gpio_if.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/gpio_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="gpio_if.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

network_common.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/network_common.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="network_common.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

pin_mux_config.obj: ../pin_mux_config.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="pin_mux_config.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_ccs.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/startup_ccs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="startup_ccs.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

test.obj: ../test.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="test.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

timer_if.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/timer_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="timer_if.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

uart_if.obj: D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common/uart_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -Ooff --opt_for_speed=4 --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/driverlib" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/inc" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/example/common" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/source" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink/include" --include_path="D:/ti/cc3200sdk/CC3200SDK_1.3.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="D:/kolin/ti/CC3200SDK/CC3200SDK_1.3.0/cc3200-sdk/example/common" --define=ccs --define=cc3200 -g --gcc --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="uart_if.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


