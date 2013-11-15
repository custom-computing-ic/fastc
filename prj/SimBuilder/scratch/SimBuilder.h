/**\file */
#ifndef SLIC_DECLARATIONS_SimBuilder_H
#define SLIC_DECLARATIONS_SimBuilder_H
#include "MaxSLiCInterface.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Simple static function for the mode 'default'
 * 
 * \param [in] ticks_Cmdhostread Scalar parameter
 * \param [in] ticks_Cmdhostwrite Scalar parameter
 * \param [in] ticks_Cmdread0 Scalar parameter
 * \param [in] ticks_Cmdread1 Scalar parameter
 * \param [in] ticks_Cmdread2 Scalar parameter
 * \param [in] ticks_Cmdread3 Scalar parameter
 * \param [in] ticks_Cmdwrite0 Scalar parameter
 * \param [in] ticks_Cmdwrite1 Scalar parameter
 * \param [in] ticks_MyApp0 Scalar parameter
 * \param [in] inscalar_Cmdhostread_Enable Scalar parameter
 * \param [in] inscalar_Cmdhostread_iniBursts Scalar parameter
 * \param [in] inscalar_Cmdhostread_totalBursts Scalar parameter
 * \param [in] inscalar_Cmdhostread_wordsPerBurst Scalar parameter
 * \param [in] inscalar_Cmdhostwrite_Enable Scalar parameter
 * \param [in] inscalar_Cmdhostwrite_iniBursts Scalar parameter
 * \param [in] inscalar_Cmdhostwrite_iterations Scalar parameter
 * \param [in] inscalar_Cmdhostwrite_totalBursts Scalar parameter
 * \param [in] inscalar_Cmdhostwrite_wordsPerBurst Scalar parameter
 * \param [in] inscalar_Cmdread0_Enable Scalar parameter
 * \param [in] inscalar_Cmdread0_iniBursts Scalar parameter
 * \param [in] inscalar_Cmdread0_totalBursts Scalar parameter
 * \param [in] inscalar_Cmdread0_wordsPerBurst Scalar parameter
 * \param [in] inscalar_Cmdread1_Enable Scalar parameter
 * \param [in] inscalar_Cmdread1_iniBursts Scalar parameter
 * \param [in] inscalar_Cmdread1_totalBursts Scalar parameter
 * \param [in] inscalar_Cmdread1_wordsPerBurst Scalar parameter
 * \param [in] inscalar_Cmdread2_Enable Scalar parameter
 * \param [in] inscalar_Cmdread2_iniBursts Scalar parameter
 * \param [in] inscalar_Cmdread2_totalBursts Scalar parameter
 * \param [in] inscalar_Cmdread2_wordsPerBurst Scalar parameter
 * \param [in] inscalar_Cmdread3_Enable Scalar parameter
 * \param [in] inscalar_Cmdread3_iniBursts Scalar parameter
 * \param [in] inscalar_Cmdread3_totalBursts Scalar parameter
 * \param [in] inscalar_Cmdread3_wordsPerBurst Scalar parameter
 * \param [in] inscalar_Cmdwrite0_Enable Scalar parameter
 * \param [in] inscalar_Cmdwrite0_iniBursts Scalar parameter
 * \param [in] inscalar_Cmdwrite0_iterations Scalar parameter
 * \param [in] inscalar_Cmdwrite0_totalBursts Scalar parameter
 * \param [in] inscalar_Cmdwrite0_wordsPerBurst Scalar parameter
 * \param [in] inscalar_Cmdwrite1_Enable Scalar parameter
 * \param [in] inscalar_Cmdwrite1_iniBursts Scalar parameter
 * \param [in] inscalar_Cmdwrite1_iterations Scalar parameter
 * \param [in] inscalar_Cmdwrite1_totalBursts Scalar parameter
 * \param [in] inscalar_Cmdwrite1_wordsPerBurst Scalar parameter
 * \param [in] inscalar_MyApp0_ORDER Scalar parameter
 * \param [in] inscalar_MyApp0_c_0 Scalar parameter
 * \param [in] inscalar_MyApp0_c_1_0 Scalar parameter
 * \param [in] inscalar_MyApp0_c_1_1 Scalar parameter
 * \param [in] inscalar_MyApp0_c_1_2 Scalar parameter
 * \param [in] inscalar_MyApp0_c_1_3 Scalar parameter
 * \param [in] inscalar_MyApp0_c_1_4 Scalar parameter
 * \param [in] inscalar_MyApp0_c_2_0 Scalar parameter
 * \param [in] inscalar_MyApp0_c_2_1 Scalar parameter
 * \param [in] inscalar_MyApp0_c_2_2 Scalar parameter
 * \param [in] inscalar_MyApp0_c_2_3 Scalar parameter
 * \param [in] inscalar_MyApp0_c_2_4 Scalar parameter
 * \param [in] inscalar_MyApp0_c_3_0 Scalar parameter
 * \param [in] inscalar_MyApp0_c_3_1 Scalar parameter
 * \param [in] inscalar_MyApp0_c_3_2 Scalar parameter
 * \param [in] inscalar_MyApp0_c_3_3 Scalar parameter
 * \param [in] inscalar_MyApp0_c_3_4 Scalar parameter
 * \param [in] inscalar_MyApp0_n1 Scalar parameter
 * \param [in] inscalar_MyApp0_n12 Scalar parameter
 * \param [in] inscalar_MyApp0_n2 Scalar parameter
 * \param [in] inscalar_MyApp0_n3 Scalar parameter
 * \param [in] instream_host2mgr stream host2mgr
 * \param [in] instream_size_host2mgr size of the stream instream_host2mgr in bytes
 * \param [out] outstream_mgr2host stream mgr2host
 * \param [in] outstream_size_mgr2host size of the stream outstream_mgr2host in bytes
 */
void SimBuilder(
	uint64_t ticks_Cmdhostread,
	uint64_t ticks_Cmdhostwrite,
	uint64_t ticks_Cmdread0,
	uint64_t ticks_Cmdread1,
	uint64_t ticks_Cmdread2,
	uint64_t ticks_Cmdread3,
	uint64_t ticks_Cmdwrite0,
	uint64_t ticks_Cmdwrite1,
	uint64_t ticks_MyApp0,
	uint64_t inscalar_Cmdhostread_Enable,
	uint64_t inscalar_Cmdhostread_iniBursts,
	uint64_t inscalar_Cmdhostread_totalBursts,
	uint64_t inscalar_Cmdhostread_wordsPerBurst,
	uint64_t inscalar_Cmdhostwrite_Enable,
	uint64_t inscalar_Cmdhostwrite_iniBursts,
	uint64_t inscalar_Cmdhostwrite_iterations,
	uint64_t inscalar_Cmdhostwrite_totalBursts,
	uint64_t inscalar_Cmdhostwrite_wordsPerBurst,
	uint64_t inscalar_Cmdread0_Enable,
	uint64_t inscalar_Cmdread0_iniBursts,
	uint64_t inscalar_Cmdread0_totalBursts,
	uint64_t inscalar_Cmdread0_wordsPerBurst,
	uint64_t inscalar_Cmdread1_Enable,
	uint64_t inscalar_Cmdread1_iniBursts,
	uint64_t inscalar_Cmdread1_totalBursts,
	uint64_t inscalar_Cmdread1_wordsPerBurst,
	uint64_t inscalar_Cmdread2_Enable,
	uint64_t inscalar_Cmdread2_iniBursts,
	uint64_t inscalar_Cmdread2_totalBursts,
	uint64_t inscalar_Cmdread2_wordsPerBurst,
	uint64_t inscalar_Cmdread3_Enable,
	uint64_t inscalar_Cmdread3_iniBursts,
	uint64_t inscalar_Cmdread3_totalBursts,
	uint64_t inscalar_Cmdread3_wordsPerBurst,
	uint64_t inscalar_Cmdwrite0_Enable,
	uint64_t inscalar_Cmdwrite0_iniBursts,
	uint64_t inscalar_Cmdwrite0_iterations,
	uint64_t inscalar_Cmdwrite0_totalBursts,
	uint64_t inscalar_Cmdwrite0_wordsPerBurst,
	uint64_t inscalar_Cmdwrite1_Enable,
	uint64_t inscalar_Cmdwrite1_iniBursts,
	uint64_t inscalar_Cmdwrite1_iterations,
	uint64_t inscalar_Cmdwrite1_totalBursts,
	uint64_t inscalar_Cmdwrite1_wordsPerBurst,
	uint64_t inscalar_MyApp0_ORDER,
	double inscalar_MyApp0_c_0,
	double inscalar_MyApp0_c_1_0,
	double inscalar_MyApp0_c_1_1,
	double inscalar_MyApp0_c_1_2,
	double inscalar_MyApp0_c_1_3,
	double inscalar_MyApp0_c_1_4,
	double inscalar_MyApp0_c_2_0,
	double inscalar_MyApp0_c_2_1,
	double inscalar_MyApp0_c_2_2,
	double inscalar_MyApp0_c_2_3,
	double inscalar_MyApp0_c_2_4,
	double inscalar_MyApp0_c_3_0,
	double inscalar_MyApp0_c_3_1,
	double inscalar_MyApp0_c_3_2,
	double inscalar_MyApp0_c_3_3,
	double inscalar_MyApp0_c_3_4,
	uint64_t inscalar_MyApp0_n1,
	uint64_t inscalar_MyApp0_n12,
	uint64_t inscalar_MyApp0_n2,
	uint64_t inscalar_MyApp0_n3,
	const void *instream_host2mgr,
	size_t instream_size_host2mgr,
	void *outstream_mgr2host,
	size_t outstream_size_mgr2host);

/**
 * \brief Advanced static interface, structure for the mode 'default'
 * 
 */
typedef struct { 
	uint64_t ticks_Cmdhostread; /**<  [in] Scalar parameter */
	uint64_t ticks_Cmdhostwrite; /**<  [in] Scalar parameter */
	uint64_t ticks_Cmdread0; /**<  [in] Scalar parameter */
	uint64_t ticks_Cmdread1; /**<  [in] Scalar parameter */
	uint64_t ticks_Cmdread2; /**<  [in] Scalar parameter */
	uint64_t ticks_Cmdread3; /**<  [in] Scalar parameter */
	uint64_t ticks_Cmdwrite0; /**<  [in] Scalar parameter */
	uint64_t ticks_Cmdwrite1; /**<  [in] Scalar parameter */
	uint64_t ticks_MyApp0; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostread_Enable; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostread_iniBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostread_totalBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostread_wordsPerBurst; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostwrite_Enable; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostwrite_iniBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostwrite_iterations; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostwrite_totalBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdhostwrite_wordsPerBurst; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread0_Enable; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread0_iniBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread0_totalBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread0_wordsPerBurst; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread1_Enable; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread1_iniBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread1_totalBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread1_wordsPerBurst; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread2_Enable; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread2_iniBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread2_totalBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread2_wordsPerBurst; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread3_Enable; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread3_iniBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread3_totalBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdread3_wordsPerBurst; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite0_Enable; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite0_iniBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite0_iterations; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite0_totalBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite0_wordsPerBurst; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite1_Enable; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite1_iniBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite1_iterations; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite1_totalBursts; /**<  [in] Scalar parameter */
	uint64_t inscalar_Cmdwrite1_wordsPerBurst; /**<  [in] Scalar parameter */
	uint64_t inscalar_MyApp0_ORDER; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_0; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_1_0; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_1_1; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_1_2; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_1_3; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_1_4; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_2_0; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_2_1; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_2_2; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_2_3; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_2_4; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_3_0; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_3_1; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_3_2; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_3_3; /**<  [in] Scalar parameter */
	double inscalar_MyApp0_c_3_4; /**<  [in] Scalar parameter */
	uint64_t inscalar_MyApp0_n1; /**<  [in] Scalar parameter */
	uint64_t inscalar_MyApp0_n12; /**<  [in] Scalar parameter */
	uint64_t inscalar_MyApp0_n2; /**<  [in] Scalar parameter */
	uint64_t inscalar_MyApp0_n3; /**<  [in] Scalar parameter */
	const void *instream_host2mgr; /**<  [in] stream host2mgr */
	size_t instream_size_host2mgr; /**<  [in] size of the stream instream_host2mgr in bytes */
	void *outstream_mgr2host; /**<  [out] stream mgr2host */
	size_t outstream_size_mgr2host; /**<  [in] size of the stream outstream_mgr2host in bytes */
} SimBuilder_actions_t;

/**
 * \brief Advanced static function for the mode 'default'
 * \param [in] engine the engine on which the actions will be executed
 * \param [in,out] mode_actions actions to be executed
 */
void SimBuilder_run(
	max_engine_t *engine,
	SimBuilder_actions_t *mode_actions);

/**
 * \brief Group run advanced static function for the mode 'default'
 * \param group group to use
 * \param mode_actions actions to run
 *
 * Run the actions on the first device available in the group.
 */
void SimBuilder_run_group(max_group_t *group, SimBuilder_actions_t *mode_actions);

/**
 * \brief Array run advanced static function for the mode 'default'
 * \param engarray the array of devices to use
 * \param mode_actions the array of actions to run
 *
 * Run the array of actions on the array of engines.  The length of mode_actions
 * must match the size of engarray.
 */
void SimBuilder_run_array(max_engarray_t *engarray, SimBuilder_actions_t *mode_actions[]);

max_actions_t* SimBuilder_convert(max_file_t *maxfile, SimBuilder_actions_t *mode_actions);

/**
 * \brief Turn debugging on for the functions in the static 
 *          interface of this maxfile.
 * \param debug_directory directory for the generated files
 */
void SimBuilder_debug(const char *debug_directory);

/**
 * \brief Initialise a maxfile.
 */
max_file_t* SimBuilder_init(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* SLIC_DECLARATIONS_SimBuilder_H */

