/*
 * app_bat_port.c
 *
 *  Created on: Jan 13, 2021
 *      Author: quangnd
 */

#include "app_bat_port.h"
#include "board.h"
int32_t OCV_table[101] = {
		53000,	53000,	53000,	53000,	53000,	53000,	55114,	55203,	55291,	55371,
		55440,	55520,	55619,	55763,	55962,	56146,	56512,	56656,	56774,	56890,
		57003,	57112,	57206,	57301,	57381,	57445,	57525,	57589,	57658,	57723,
		57778,	57827,	57877,	57915,	57960,	58005,	58050,	58090,	58139,	58179,
		58224,	58278,	58323,	58373,	58422,	58472,	58531,	58590,	58650,	58715,
		58784,	58858,	58942,	59037,	59155,	59299,	59469,	59662,	59845,	60010,
		60142,	60272,	60402,	60530,	60659,	60794,	60931,	61070,	61210,	61349,
		61493,	61642,	61790,	62093,	62246,	62405,	62563,	62723,	62882,	63050,
		63213,	63378,	63546,	63715,	63883,	64067,	64245,	64429,	64608,	64800,
		64984,	65173,	65366,	65565,	65763,	65971,	66179,	66394,	66621,	66854,	67117
};
volatile uint32_t unbalance_cnt;
volatile uint32_t switch_cnt;

static Bat_port bs_ports[BAT_PORT_NUM];

typedef void (*sw_act)(Switch* p_sw);

static void bat1_on(Switch* p_sw);
static void bat2_on(Switch* p_sw);
static void bat3_on(Switch* p_sw);
static void bat1_off(Switch* p_sw);
static void bat2_off(Switch* p_sw);
static void bat3_off(Switch* p_sw);
static void	check_working_port(pmu_app*);


static sw_act bat_on_interface[] = {bat1_on, bat2_on, bat3_on};
static sw_act bat_off_interface[] = {bat1_off, bat2_off, bat3_off};

//static sw_act bat_on_interface[] = {bat2_on, bat3_on, bat1_on};
//static sw_act bat_off_interface[] = {bat2_off, bat3_off, bat1_off};

static void 	bat_port_set_power_sw_handle(Bat_port*, BMS_SWICTH_STATE, CM_CO_SERVICE );

void app_bat_ports_init(pmu_app* p_app){
	p_app->bat_port_num = BAT_PORT_NUM;
	p_app->sw_type = BS_NO_SWITCH;
	//p_app->mode = BS_DISCHARGE;
	p_app->working_port_num = 0;
	p_app->is_wakeup = false;
	p_app->working_port_discharge_num = 0;
	p_app->ports = &bs_ports[0];
	p_app->check_working_port_num = check_working_port;
	p_app->sw_timeout = 0;
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		p_app->working_port[i] = NULL;
		bat_port_init(&bs_ports[i], i);
		bs_ports[i].node_id.sw_on = bat_on_interface[i];
		bs_ports[i].node_id.sw_off = bat_off_interface[i];
		bs_ports[i].set_power_sw = bat_port_set_power_sw_handle;
	}

}

static void	check_working_port(pmu_app* p_app){
	if(p_app->working_port_num >= 3) p_app->working_port_num = 3;
	else if(p_app->working_port_num <= 0) p_app->working_port_num = 0;
}

static void bat1_on(Switch* p_sw){
	p_sw->state = SW_ST_ON;
	HAL_NODE_ID_SET_HIGH(NODEID_1);
}

static void bat2_on(Switch* p_sw){
	p_sw->state = SW_ST_ON;
	HAL_NODE_ID_SET_HIGH(NODEID_2);
}

static void bat3_on(Switch* p_sw){
	p_sw->state = SW_ST_ON;
	HAL_NODE_ID_SET_HIGH(NODEID_3);
}

static void bat1_off(Switch* p_sw){
	p_sw->state = SW_ST_OFF;
	HAL_NODE_ID_SET_LOW(NODEID_1);
}

static void bat2_off(Switch* p_sw){
	p_sw->state = SW_ST_OFF;
	HAL_NODE_ID_SET_LOW(NODEID_2);
}

static void bat3_off(Switch* p_sw){
	p_sw->state = SW_ST_OFF;
	HAL_NODE_ID_SET_LOW(NODEID_3);
}

static void bat_port_set_power_sw_handle(Bat_port* p_port, BMS_SWICTH_STATE sw_state, CM_CO_SERVICE co_service){
	if(CO_SDO_get_status(&selex_bs_app.base.co_app.sdo_client) != CO_SDO_RT_idle) return;
	bp_set_main_pre_sw_index(p_port->bp, sw_state);
	selex_bs_app.impl_port = p_port;
	selex_bs_app.base.co_service = co_service;
	co_sdo_write_object((CAN_master*)&selex_bs_app,
			BMS_MAINSWITCH_INDEX,
			p_port->bp->base.node_id,
			(uint8_t*)&p_port->bp->pre_sw_state,
			1, sys_timestamp + BMS_MAINSWITCH_SDO_TIMEOUT_mS);
}
/**
 * Send max total current to MC that the Bp can provide
 * cur_max = (ocv-V_BP_min)/res_BP;
 * when soc < 20%: cur_max =cur_max-5A
 * resistance 1 BP:150mohm
 * resistance 2 BP:80mohm
 * resistance 3 BP:60mohm
 * unbalance: --> cur_max-20A
 */
bool over_heat_cur_limit = false;
void cal_max_cur_dc_bus(pmu_app* p_app ){
	if (p_app->is_new_bp_data == false) return;
	p_app->is_new_bp_data = false;
	if ( unbalance_cnt-- > 2) return;
	unbalance_cnt = 1;
	int32_t num = 0;
	int32_t res_BP =150;//resistance 1 BP
	int32_t cur_max =0;
	int32_t soc =0;
	int32_t soc_old =0;
	int32_t ocv =0;
	uint8_t unbalance = 0;
	uint8_t bat_port = 0;
	for (int i=0; i < p_app-> bat_port_num; i++){
		if (p_app->ports[i].bp->state == BP_ST_DISCHARGING){
			bat_port = i;
			num++;
			soc_old = soc;
			soc += p_app->ports[i].bp->soc;
			unbalance += check_unbalance_BP( p_app->ports[i].bp->cell_vol) ;
			if ( soc < 1 ) soc = 1;
			if ((soc > soc_old) && (soc_old != 0) ) soc = soc_old;
		}
	}
	if ( soc < 1 ) soc = 1;
	if ( soc > 99 ) soc = 99;
	ocv = OCV_table[soc];
	if (num == 1) 		res_BP =150;
	else if (num == 2 ) res_BP =90;
	else if (num == 3 )	res_BP =60;
	cur_max = 1000*(ocv-53000)/res_BP;
	switch(num){
	case 1:
		if ( soc <= 20 ) {cur_max = cur_max - 2000;}
		if ( cur_max >= 35000) 	{cur_max = 35000;}
		if ( cur_max <= 5000) 	{cur_max = 5000 ;}
		/* active overheat cur limit level 1*/
		if((p_app->ports[bat_port].bp->temp[0]) > 55 && (cur_max > 20000)){
			cur_max = 20000 ;
			over_heat_cur_limit = true;
		}
#if 0
		/* active overheat cur limit level 2*/
		if((p_app->ports[bat_port].bp->temp[0]) > 60 && (cur_max > 15000)){
			cur_max = 15000 ;
			break;
		}
#endif
		/* hyteresis overheat cur limit*/
		if((over_heat_cur_limit == true) &&
				((p_app->ports[bat_port].bp->temp[0]) > 54) &&
				(cur_max > 20000) ){
			cur_max = 20000 ;
		}
		else{
			over_heat_cur_limit = false;
		}
		break;
	case 2:
		if ( soc <= 20 ) {cur_max = cur_max - 2000;}
		if ( cur_max >= 45000) 	{cur_max = 45000;}
		if ( cur_max <= 1000) 	{cur_max = 10000;}
		break;
	case 3:
		if ( soc <= 20 ) {cur_max = cur_max - 2000;}
		if ( cur_max >= 50000) 	{cur_max = 50000;}
		if ( cur_max <= 15000) 	{cur_max = 15000;}
		break;
	default:
		break;
	}
	p_app->cur_max_bp = cur_max;
}
void cal_power(Bat_port* port, float delta_t ){
	if( port == NULL) return;
	if(		port->bp->vol > 70000 ||
			port->bp->vol < 40000 ||
			port->bp->cur < 0	  ||
			port->bp->cur > 80000) {
		return;
	}
	float V_bat = (float)port->bp->vol / 1000.0f;
	float I_bat = (float)port->bp->cur / 1000.0f;
	port->bp->power_discharge += V_bat*I_bat * delta_t/3600;

}
