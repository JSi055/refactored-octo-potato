#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/refactored-octo-potato.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/refactored-octo-potato.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=Final_Project_main.c lonne025_delays_v002.s CUU_M68_4bit_ported.c Fake_Arduino.c Noritake_VFD_CUU_ported.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Final_Project_main.o ${OBJECTDIR}/lonne025_delays_v002.o ${OBJECTDIR}/CUU_M68_4bit_ported.o ${OBJECTDIR}/Fake_Arduino.o ${OBJECTDIR}/Noritake_VFD_CUU_ported.o
POSSIBLE_DEPFILES=${OBJECTDIR}/Final_Project_main.o.d ${OBJECTDIR}/lonne025_delays_v002.o.d ${OBJECTDIR}/CUU_M68_4bit_ported.o.d ${OBJECTDIR}/Fake_Arduino.o.d ${OBJECTDIR}/Noritake_VFD_CUU_ported.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/Final_Project_main.o ${OBJECTDIR}/lonne025_delays_v002.o ${OBJECTDIR}/CUU_M68_4bit_ported.o ${OBJECTDIR}/Fake_Arduino.o ${OBJECTDIR}/Noritake_VFD_CUU_ported.o

# Source Files
SOURCEFILES=Final_Project_main.c lonne025_delays_v002.s CUU_M68_4bit_ported.c Fake_Arduino.c Noritake_VFD_CUU_ported.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/refactored-octo-potato.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ64GA002
MP_LINKER_FILE_OPTION=,--script=p24FJ64GA002.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Final_Project_main.o: Final_Project_main.c  .generated_files/flags/default/541a8c44d8f5bfef88bac3ed8a9dffa032869121 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Final_Project_main.o.d 
	@${RM} ${OBJECTDIR}/Final_Project_main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Final_Project_main.c  -o ${OBJECTDIR}/Final_Project_main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Final_Project_main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/CUU_M68_4bit_ported.o: CUU_M68_4bit_ported.c  .generated_files/flags/default/b782bbce9adf804261023c23dbcf74a119bfe8d0 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/CUU_M68_4bit_ported.o.d 
	@${RM} ${OBJECTDIR}/CUU_M68_4bit_ported.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU_M68_4bit_ported.c  -o ${OBJECTDIR}/CUU_M68_4bit_ported.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU_M68_4bit_ported.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Fake_Arduino.o: Fake_Arduino.c  .generated_files/flags/default/8adc46b7120a9137c224dee9e86a6202719d83d9 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Fake_Arduino.o.d 
	@${RM} ${OBJECTDIR}/Fake_Arduino.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Fake_Arduino.c  -o ${OBJECTDIR}/Fake_Arduino.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Fake_Arduino.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Noritake_VFD_CUU_ported.o: Noritake_VFD_CUU_ported.c  .generated_files/flags/default/1b1f724f6ad637871b396f7621cce478b598ea38 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Noritake_VFD_CUU_ported.o.d 
	@${RM} ${OBJECTDIR}/Noritake_VFD_CUU_ported.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Noritake_VFD_CUU_ported.c  -o ${OBJECTDIR}/Noritake_VFD_CUU_ported.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Noritake_VFD_CUU_ported.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/Final_Project_main.o: Final_Project_main.c  .generated_files/flags/default/3735b2cb5aba5f86ee66cbbdfd2e5c8625e2a371 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Final_Project_main.o.d 
	@${RM} ${OBJECTDIR}/Final_Project_main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Final_Project_main.c  -o ${OBJECTDIR}/Final_Project_main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Final_Project_main.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/CUU_M68_4bit_ported.o: CUU_M68_4bit_ported.c  .generated_files/flags/default/9b610969be9efd3644955199d83f28a94ade2274 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/CUU_M68_4bit_ported.o.d 
	@${RM} ${OBJECTDIR}/CUU_M68_4bit_ported.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU_M68_4bit_ported.c  -o ${OBJECTDIR}/CUU_M68_4bit_ported.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU_M68_4bit_ported.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Fake_Arduino.o: Fake_Arduino.c  .generated_files/flags/default/2b5160380d31bd7cf8624ad7bd1966e59d1bc40f .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Fake_Arduino.o.d 
	@${RM} ${OBJECTDIR}/Fake_Arduino.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Fake_Arduino.c  -o ${OBJECTDIR}/Fake_Arduino.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Fake_Arduino.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Noritake_VFD_CUU_ported.o: Noritake_VFD_CUU_ported.c  .generated_files/flags/default/e1e73a0b1ee2a493cb90a1663ec401bcec8a3e8b .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Noritake_VFD_CUU_ported.o.d 
	@${RM} ${OBJECTDIR}/Noritake_VFD_CUU_ported.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Noritake_VFD_CUU_ported.c  -o ${OBJECTDIR}/Noritake_VFD_CUU_ported.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Noritake_VFD_CUU_ported.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/lonne025_delays_v002.o: lonne025_delays_v002.s  .generated_files/flags/default/5f2157c6faa3fc2976acaa33b2a0325136f3b9df .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/lonne025_delays_v002.o.d 
	@${RM} ${OBJECTDIR}/lonne025_delays_v002.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  lonne025_delays_v002.s  -o ${OBJECTDIR}/lonne025_delays_v002.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -omf=elf -DXPRJ_default=$(CND_CONF)    -Wa,-MD,"${OBJECTDIR}/lonne025_delays_v002.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/lonne025_delays_v002.o: lonne025_delays_v002.s  .generated_files/flags/default/ce189128568f3cc8142ce8a8ba6517279c44c4b0 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/lonne025_delays_v002.o.d 
	@${RM} ${OBJECTDIR}/lonne025_delays_v002.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  lonne025_delays_v002.s  -o ${OBJECTDIR}/lonne025_delays_v002.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -DXPRJ_default=$(CND_CONF)    -Wa,-MD,"${OBJECTDIR}/lonne025_delays_v002.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp="${DFP_DIR}/xc16"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/refactored-octo-potato.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/refactored-octo-potato.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)      -Wl,,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
${DISTDIR}/refactored-octo-potato.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/refactored-octo-potato.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}/xc16-bin2hex ${DISTDIR}/refactored-octo-potato.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
