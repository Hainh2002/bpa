/*
 * test_suit.c
 *
 *  Created on: Jun 23, 2020
 *      Author: quangnd
 */

#include "../../../libs/selex-libc/test-suit/test_suit.h"

static Test_Suit test_suit;
static uint16_t test_index=0;

void test_suit_init(Test_Report report_handle){
	test_suit.report=report_handle;
};

void test_suit_add_test(Test_Case* p_test){
	test_suit.tests[test_index]=p_test;
	test_enable(p_test);
}

void test_suit_remove_test(Test_Case* p_test){

	for(int i=0;i<test_index;i++){
		if(test_suit.tests[i]==p_test){
			for(int j=0;j<test_index;j++){
				test_suit.tests[j]=test_suit.tests[j+1];
			}
			test_index--;
			break;
		}
	}
}

void test_suit_enable_test(Test_Case* p_test){

	for(int i=0;i<test_index;i++){
		if(test_suit.tests[i]==p_test){
			test_enable(test_suit.tests[i]);
			break;
		}
	}
}

void test_suit_ignore_test(Test_Case* p_test){

	for(int i=0;i<test_index;i++){
		if(test_suit.tests[i]==p_test){
			test_ignore(test_suit.tests[i]);
			break;
		}
	}
}

void test_suit_run(void){

	for(int i=0;i<test_index;i++){
		test_setup(test_suit.tests[i]);
		test_run(test_suit.tests[i]);
		test_finish(test_suit.tests[i]);
		test_suit.report(test_suit.tests[i]);
	}
}



