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
SOURCEFILES_QUOTED_IF_SPACED=CUU/Noritake_VFD_CUU.c Final_Project_main.c CUU/Fake_Arduino.c CUU/Noritake_VFD_CUU_1.c CUU/CUU_M68_4bit_ported.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/CUU/Noritake_VFD_CUU.o ${OBJECTDIR}/Final_Project_main.o ${OBJECTDIR}/CUU/Fake_Arduino.o ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o
POSSIBLE_DEPFILES=${OBJECTDIR}/CUU/Noritake_VFD_CUU.o.d ${OBJECTDIR}/Final_Project_main.o.d ${OBJECTDIR}/CUU/Fake_Arduino.o.d ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o.d ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/CUU/Noritake_VFD_CUU.o ${OBJECTDIR}/Final_Project_main.o ${OBJECTDIR}/CUU/Fake_Arduino.o ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o

# Source Files
SOURCEFILES=CUU/Noritake_VFD_CUU.c Final_Project_main.c CUU/Fake_Arduino.c CUU/Noritake_VFD_CUU_1.c CUU/CUU_M68_4bit_ported.c



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
${OBJECTDIR}/CUU/Noritake_VFD_CUU.o: CUU/Noritake_VFD_CUU.c  .generated_files/flags/default/463c0cc5c82bc253eeba5f450a56791bb6d70ddf .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}/CUU" 
	@${RM} ${OBJECTDIR}/CUU/Noritake_VFD_CUU.o.d 
	@${RM} ${OBJECTDIR}/CUU/Noritake_VFD_CUU.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU/Noritake_VFD_CUU.c  -o ${OBJECTDIR}/CUU/Noritake_VFD_CUU.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU/Noritake_VFD_CUU.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Final_Project_main.o: Final_Project_main.c  .generated_files/flags/default/541a8c44d8f5bfef88bac3ed8a9dffa032869121 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Final_Project_main.o.d 
	@${RM} ${OBJECTDIR}/Final_Project_main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Final_Project_main.c  -o ${OBJECTDIR}/Final_Project_main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Final_Project_main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/CUU/Fake_Arduino.o: CUU/Fake_Arduino.c  .generated_files/flags/default/6bd3986648034325c68b8fe3f865566f2ec7ae16 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}/CUU" 
	@${RM} ${OBJECTDIR}/CUU/Fake_Arduino.o.d 
	@${RM} ${OBJECTDIR}/CUU/Fake_Arduino.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU/Fake_Arduino.c  -o ${OBJECTDIR}/CUU/Fake_Arduino.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU/Fake_Arduino.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o: CUU/Noritake_VFD_CUU_1.c  .generated_files/flags/default/2b5fcdbd2b816ce61c701d149a3d04c444e98f4d .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}/CUU" 
	@${RM} ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o.d 
	@${RM} ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU/Noritake_VFD_CUU_1.c  -o ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o: CUU/CUU_M68_4bit_ported.c  .generated_files/flags/default/4af981940f65b7c93cd3b2c44b5e19d6e5df7868 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}/CUU" 
	@${RM} ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o.d 
	@${RM} ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU/CUU_M68_4bit_ported.c  -o ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/CUU/Noritake_VFD_CUU.o: CUU/Noritake_VFD_CUU.c  .generated_files/flags/default/28d280c4ab085575f479b7522dad8ddc4f03132d .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}/CUU" 
	@${RM} ${OBJECTDIR}/CUU/Noritake_VFD_CUU.o.d 
	@${RM} ${OBJECTDIR}/CUU/Noritake_VFD_CUU.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU/Noritake_VFD_CUU.c  -o ${OBJECTDIR}/CUU/Noritake_VFD_CUU.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU/Noritake_VFD_CUU.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Final_Project_main.o: Final_Project_main.c  .generated_files/flags/default/3735b2cb5aba5f86ee66cbbdfd2e5c8625e2a371 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Final_Project_main.o.d 
	@${RM} ${OBJECTDIR}/Final_Project_main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Final_Project_main.c  -o ${OBJECTDIR}/Final_Project_main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Final_Project_main.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/CUU/Fake_Arduino.o: CUU/Fake_Arduino.c  .generated_files/flags/default/37813bf955dc848d392ac2addfd96ed1a7d47327 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}/CUU" 
	@${RM} ${OBJECTDIR}/CUU/Fake_Arduino.o.d 
	@${RM} ${OBJECTDIR}/CUU/Fake_Arduino.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU/Fake_Arduino.c  -o ${OBJECTDIR}/CUU/Fake_Arduino.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU/Fake_Arduino.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o: CUU/Noritake_VFD_CUU_1.c  .generated_files/flags/default/3309ac4ca42049e6875dfc85968d11e3bcfa8b8d .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}/CUU" 
	@${RM} ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o.d 
	@${RM} ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU/Noritake_VFD_CUU_1.c  -o ${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU/Noritake_VFD_CUU_1.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o: CUU/CUU_M68_4bit_ported.c  .generated_files/flags/default/d348a99cbb5bd1cea8d24885844a9f7e02aae156 .generated_files/flags/default/ab79f85e8b884ca16bf55aa7d0cf27f9007bd660
	@${MKDIR} "${OBJECTDIR}/CUU" 
	@${RM} ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o.d 
	@${RM} ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  CUU/CUU_M68_4bit_ported.c  -o ${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/CUU/CUU_M68_4bit_ported.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
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
