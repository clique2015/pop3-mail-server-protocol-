#pragma once

#define APP_TITLE "pop3server"
#define APP_VERSION "1.0"
#define client_message_SIZE 1024
#define DOMAIN_ROOT_PATH "mails/"
#define USER_LIST "./list.txt"

//POP3 Defines

#define POP3_PORT 110

#define POP3_STATE_AUTHORIZATION 1
#define POP3_STATE_RPOP 2
#define POP3_STATE_TRANSACTION 3
#define POP3_STATE_UPDATE 4

#define POP3_DEFAULT_NEGATIVE_RESPONSE 0
#define POP3_DEFAULT_AFFERMATIVE_RESPONSE 1

#define POP3_STAT_RESPONSE 16
#define POP3_WELCOME_RESPONSE 2
#define POP3_MSG_STATUS_UNDEFINED 0
#define POP3_MSG_STATUS_NEW 1
#define POP3_MSG_STATUS_READ 2
#define POP3_MSG_STATUS_REPLIED 4
#define POP3_MSG_STATUS_DELETED 8
#define POP3_MSG_STATUS_CUSTOM 16


//SMTP Defines

#define SMTP_PORT 25

#define SMTP_DATA_TERMINATOR "\r\n.\r\n"