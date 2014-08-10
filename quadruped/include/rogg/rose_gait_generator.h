/**
 * @file rose_gait_generator.h
 * Header for \ref rose_gait_generator.cpp
 */

#ifndef ROSEGAITGENERATOR_INCLUDE_ROSE_GAIT_GENERATOR_H_
#define ROSEGAITGENERATOR_INCLUDE_ROSE_GAIT_GENERATOR_H_

#include <stdio.h>
#include "include/rogg/gaitgenerator.h"
#include "include/rogg/log.h"
#include "include/rogg/hmatrix.h"

namespace ROGG {

extern "C" int Initialize();
extern "C" int Terminate();
extern "C" int InitializeRun();
extern "C" int TerminateRun();
extern "C" int myFunction(double* inarr, int inputs, double* outarr,
                          int outputs, int major);
extern "C" int Step(double* inarr, int inputs, double* outarr,
                    int outputs, int major);
extern "C" int SetParHL1(double* inarr, int inputs, double* outarr,
                          int outputs, int major);
extern "C" int SetParHL2(double* inarr, int inputs, double* outarr,
                          int outputs, int major);
extern "C" int SetParH_cob_pivot10(double* inarr, int inputs, double* outarr,
                                   int outputs, int major);
extern "C" int SetParH_cob_pivot20(double* inarr, int inputs, double* outarr,
                                   int outputs, int major);
extern "C" int SetParH_cob_pivot30(double* inarr, int inputs, double* outarr,
                                   int outputs, int major);
extern "C" int SetParH_cob_pivot40(double* inarr, int inputs, double* outarr,
                                   int outputs, int major);
extern "C" int SetParASM_min(double* inarr, int inputs, double* outarr,
                                   int outputs, int major);
extern "C" int SetParTransferSpeeds(double* inarr, int inputs, double* outarr,
                                    int outputs, int major);
extern "C" int SetParGroundClearance(double* inarr, int inputs, double* outarr,
                                     int outputs, int major);
extern "C" int SetVarVelocity(double* inarr, int inputs, double* outarr,
                              int outputs, int major);
extern "C" int EnterTransferState(double* inarr, int inputs, double* outarr,
                                      int outputs, int major);
extern "C" int ExitTransferState(double* inarr, int inputs, double* outarr,
                                 int outputs, int major);
extern "C" int SetVarP1(double* inarr, int inputs, double* outarr,
                        int outputs, int major);
extern "C" int SetVarP2(double* inarr, int inputs, double* outarr,
                        int outputs, int major);
extern "C" int SetVarP3(double* inarr, int inputs, double* outarr,
                        int outputs, int major);
extern "C" int SetVarP4(double* inarr, int inputs, double* outarr,
                        int outputs, int major);
extern "C" int GetKM(double* inarr, int inputs, double* outarr,
                     int outputs, int major);
extern "C" int GetLASMBF(double* inarr, int inputs, double* outarr,
                         int outputs, int major);
extern "C" int GetDebugInfo(double* inarr, int inputs, double* outarr,
                            int outputs, int major);
extern "C" int GetTargetFoothold(double* inarr, int inputs, double* outarr,
                                 int outputs, int major);
extern "C" int GetTargetLeg(double* inarr, int inputs, double* outarr,
                            int outputs, int major);
extern "C" int GetSearchArea(double* inarr, int inputs, double* outarr,
                            int outputs, int major);
extern "C" int GetFullTDArea(double* inarr, int inputs, double* outarr,
                            int outputs, int major);
extern "C" int GetFullKMArea(double* inarr, int inputs, double* outarr,
                            int outputs, int major);
extern "C" int DisableLog(double* inarr, int inputs, double* outarr,
                          int outputs, int major);
extern "C" int DisableWavegaitEnforcement(double* inarr, int inputs,
                                          double* outarr, int outputs,
                                          int major);
extern "C" int EnableWavegaitEnforcement(double* inarr, int inputs,
                                         double* outarr, int outputs,
                                         int major);
extern "C" int DisableSelectionMargin(double* inarr, int inputs,
                                      double* outarr, int outputs,
                                      int major);
extern "C" int EnableSelectionMargin(double* inarr, int inputs,
                                      double* outarr, int outputs,
                                      int major);
extern "C" int DisableForceWavegait(double* inarr, int inputs,
                                      double* outarr, int outputs,
                                      int major);
extern "C" int EnableForceWavegait(double* inarr, int inputs,
                                      double* outarr, int outputs,
                                      int major);
extern "C" int SetSearchWidth(double* inarr, int inputs, double* outarr, 
                              int outputs, int major);

}  // namespace ROGG

#endif  // ROSEGAITGENERATOR_INCLUDE_ROSE_GAIT_GENERATOR_H_
