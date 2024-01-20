/*
 * test_suit.h
 *
 *  Created on: Jun 23, 2020
 *      Author: quangnd
 */

#ifndef LIBS_TEST_SUIT_TEST_SUIT_H_
#define LIBS_TEST_SUIT_TEST_SUIT_H_

#include "stdint.h"

#define TEST_PASS						1L
#define TEST_FAIL						0L
#define TEST_ENABLED					1L
#define TEST_IGNORED					0L

#define MAX_TEST_CASE					128

typedef struct Test_Case_t Test_Case;
typedef struct Test_Suit_t Test_Suit;
typedef struct ITest_Case_t ITest_Case;
typedef void (*Test_Report)(Test_Case* p_test);

struct ITest_Case_t{
	void (*setup)(Test_Case* p_test);
	int32_t (*run)(Test_Case* p_test);
	void (*finish)(Test_Case* p_test);
};

struct Test_Case_t{
	ITest_Case* interface;
	uint8_t active;
	int32_t result;
};

struct Test_Suit_t{
	Test_Case* tests[MAX_TEST_CASE];
	void (*report)(Test_Case* p_test);
};

static inline int32_t test_run(Test_Case* p_test){
	p_test->result=p_test->interface->run(p_test);
	return p_test->result;
}

static inline void test_setup(Test_Case* p_test){
	p_test->interface->setup(p_test);
}

static inline void test_finish(Test_Case* p_test){
	p_test->interface->finish(p_test);
}

static inline void test_enable(Test_Case* p_test){
	p_test->active=TEST_ENABLED;
}

static inline void test_ignore(Test_Case* p_test){

	p_test->active=TEST_IGNORED;
}

static inline void test_set_interface(Test_Case* p_test, ITest_Case* p_interface){
	p_test->interface=p_interface;
}

void test_suit_add_test(Test_Case* p_test);
void test_suit_remove_test(Test_Case* p_test);
void test_suit_enable_test(Test_Case* p_test);
void test_suit_ignore_test( Test_Case* p_test);
void test_suit_run(void);
void test_suit_init(Test_Report report_handle);

#endif /* LIBS_TEST_SUIT_TEST_SUIT_H_ */
