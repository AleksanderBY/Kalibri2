

#ifndef _CLIE0DEF_H    /* nested Headers management */
#define _CLIE0DEF_H

#define READ_NWORDS	 	3
#define WRITE_1WORD 	6
#define WRITE_NWORDS 16

#define ANALOG_DB          0
#define METER_DB           1
#define STATUS_DB          2


//запрос Науцилуса
#define R_NAU								100

//подзапроcы Науцилуса
#define NAU_R_ANY						1
#define NAU_R_INTERN				2
#define NAU_W_ANY						3
#define NAU_W_INTERN				4


#define NET_MAX_SLQ					50000

/*application status*/
#define APP_RUN  		0
#define APP_STOP 		1
#define APP_DBFAIL 	2

#define MIN_RESP_LEN 5

quint16 usr_getva (quint16 usad, uchar *typ, uchar *attr);
uchar  check_app (uchar slave);
quint16 rtd_com_question (uchar *ques, uchar *ans, qint32 time_out);
uchar* com_link(uchar slave);
uchar  usr_spc_init (uchar slave);

void   rtd_open_list(uchar *buf);
uchar  rtd_status(uchar *buf);
void   rtd_close_list();
void   rtd_add_list(quint16 va);
uchar  rtd_status_list(uchar *buf);
uchar  rtd_get_list(void *val);
void   rtd_open_wlist(uchar *buf);
void   rtd_close_wlist();
void   rtd_add_wlist(quint16 va, void *val, uchar attr);

qint16 tcp_request (int sock, uchar slv, uchar n, quint16 addr);
extern "C" qint16 tcp_get_ana (int sock, uchar slv, quint16 addr, quint16 *val);
extern "C" qint16 tcp_get_boo (int sock, uchar slv, quint16 addr, uchar *val);
extern "C" uchar tcp_isa_connect (int *sock, struct hostent	*hp, int port);
qint16 tcp_get_nvar (int sock, uchar slv, quint16 addr, int n, uchar *buf);
qint16 tcp_put_ana (int sock, uchar slv, quint16 addr, quint16 val);
extern "C" qint16 tcp_put_boo (int sock, uchar slv, quint16 addr, uchar val);
qint16 tcp_send_question (quint32 sock, uchar *question_buffer,
            uchar *answer_buffer,	quint32 a_size, quint32 q_size);
qint16 tcp_send_write (quint32 sock, uchar *question_buffer,
            uchar *answer_buffer,	quint32 a_size, quint32 q_size);
void	rtd_get_tcp (void *val);
uchar rtd_status_tcp (uchar *buf);
int		ping (struct hostent *hp, int datasize, int p_timeout);

uchar net_status(uchar *buf);
extern "C" void net_open_list(uchar *buf, uchar slave, int Intern);
extern "C" void net_close_list();
extern "C" void net_add_list(quint16 addr);
extern "C" uchar net_status_list(uchar *buf);
extern "C" uchar net_get_list(void *val);
extern "C" qint16 net_write_val(uchar * buf, uchar slave, quint16 addr, uchar *val, uchar typ, uchar Intern);
extern "C" void net_open_wlist(uchar *buf, uchar slave, int Intern);
extern "C" void net_add_wlist(quint16 addr, void *val, uchar attr);

extern "C" qint16 net_tcp_request (int sock, uchar* question_buf, uchar* answer_buf);

/*errors*/

#define ERR_OK			0		/*no error										*/
#define ERR_SOCK		1		/*error open socket						*/
#define ERR_CONNECT	2		/*can't connect								*/
#define ERR_RAW			3		/*can't open SOCK_RAW	socket	*/
#define ERR_HOST		4		/*can't find host							*/

#endif 
/* nested Headers management */
