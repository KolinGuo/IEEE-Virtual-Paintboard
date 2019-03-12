################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
camera_app.obj: ../camera_app.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv4 --code_state=32 --include_path="D:/kolin/UCD/2. Sophomore/2018 Spring/EEC 172/Lab Projects/Lab 6/Lab6" --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="camera_app.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

fs.obj: ../fs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv4 --code_state=32 --include_path="D:/kolin/UCD/2. Sophomore/2018 Spring/EEC 172/Lab Projects/Lab 6/Lab6" --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="fs.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv4 --code_state=32 --include_path="D:/kolin/UCD/2. Sophomore/2018 Spring/EEC 172/Lab Projects/Lab 6/Lab6" --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

pin_mux_config.obj: ../pin_mux_config.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv4 --code_state=32 --include_path="D:/kolin/UCD/2. Sophomore/2018 Spring/EEC 172/Lab Projects/Lab 6/Lab6" --include_path="D:/ti/ccs800/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="pin_mux_config.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


