#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <check.h>
#include <stdlib.h>
#include <time.h>

#include "../toxcore/group_announce.h"
#include "../toxcore/util.h"

#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32)
#define c_sleep(x) Sleep(1*x)
#else
#include <unistd.h>
#define c_sleep(x) usleep(1000*x)
#endif

/* Not to define it in the header */
struct ANNOUNCE {
    DHT *dht;
    Announced_node_format announced_nodes[MAX_ANNOUNCED_NODES];
    Ping_Array  ping_array;
    uint64_t    last_to_ping;
};


void do_announce(ANNOUNCE *announce)
{
    networking_poll(announce->dht->net);
    do_DHT(announce->dht);
}

void print_client_id(uint8_t *client_id)
{
    uint32_t j;

    for (j = 0; j < CLIENT_ID_SIZE; j++) {
        printf("%02hhX", client_id[j]);
    }
    printf("\n");
}

START_TEST(test_basic)
{
    IP ip;
    ip_init(&ip, 1);
    ip.ip6.uint8[15] = 1;

    ANNOUNCE *announce1 = new_announce(new_DHT(new_networking(ip, 34567)));
    ANNOUNCE *announce2 = new_announce(new_DHT(new_networking(ip, 34568)));
    ANNOUNCE *announce3 = new_announce(new_DHT(new_networking(ip, 34569)));    
    ck_assert_msg((announce1 != NULL) && (announce2 != NULL) && (announce3 != NULL), "ANNOUNCE failed initializing.");

    //networking_registerhandler(announce2->dht->net, NET_PACKET_ANNOUNCE_REQUEST, &handle_test_1, announce2->dht);
    //networking_registerhandler(announce2->dht->net, NET_PACKET_GET_ANNOUNCED_NODES, &handle_test_2, announce2->dht);
    //networking_registerhandler(announce3->dht->net, NET_PACKET_SEND_ANNOUNCED_NODES, &handle_test_3, announce3->dht);

    IP_Port on1 = {ip, announce1->dht->net->port};
    Node_format n1;
    memcpy(n1.client_id, announce1->dht->self_public_key, crypto_box_PUBLICKEYBYTES);
    n1.ip_port = on1;

    IP_Port on2 = {ip, announce2->dht->net->port};
    Node_format n2;
    memcpy(n2.client_id, announce2->dht->self_public_key, crypto_box_PUBLICKEYBYTES);
    n2.ip_port = on2;

    IP_Port on3 = {ip, announce3->dht->net->port};
    Node_format n3;
    memcpy(n3.client_id, announce3->dht->self_public_key, crypto_box_PUBLICKEYBYTES);
    n3.ip_port = on3;

    printf("Node_ID 1: ");
    print_client_id(n1.client_id);
    printf("Node_ID 2: ");
    print_client_id(n2.client_id);
    printf("Node_ID 3: ");
    print_client_id(n3.client_id);

    uint8_t chat_public_key[crypto_box_PUBLICKEYBYTES];
    uint8_t chat_private_key[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(chat_public_key, chat_private_key);

    printf("Chat_ID: ");
    print_client_id(chat_public_key);

    int result = send_gc_announce_request(announce1->dht, n2.ip_port, n2.client_id, chat_public_key);
    ck_assert_msg(result == 0, "Failed to create/send group announce request packet.");

    uint32_t i;
    for (i=0; i<20*1000; i+=50)
    {
        do_announce(announce2);
    }

    result = get_gc_announced_nodes_request(announce3->dht, n2.ip_port, n2.client_id, chat_public_key);
    ck_assert_msg(result == 0, "Failed to create/send group announce nodes request packet.");

    for (i=0; i<20*1000; i+=50)
    {
        do_announce(announce2);
        do_announce(announce3);
    }

    Node_format nodes_list[MAX_ANNOUNCED_NODES];

    //announce3->dht->announce - the fack is that??
    uint32_t num = get_announced_nodes(announce3->dht->announce, chat_public_key, nodes_list, 1);
    ck_assert_msg(num == 1, "Failed to found announced nodes");

    printf("Found nodes number: %u\n", num);
    for (i=0; i<num; i++) {
        printf("Found Node_ID: ");
        print_client_id(nodes_list[i].client_id);
    }
    
}
END_TEST

#define DEFTESTCASE(NAME) \
    TCase *tc_##NAME = tcase_create(#NAME); \
    tcase_add_test(tc_##NAME, test_##NAME); \
    suite_add_tcase(s, tc_##NAME);

#define DEFTESTCASE_SLOW(NAME, TIMEOUT) \
    DEFTESTCASE(NAME) \
    tcase_set_timeout(tc_##NAME, TIMEOUT);

Suite *announce_suite(void)
{
    Suite *s = suite_create("ANNOUNCE");

    DEFTESTCASE_SLOW(basic, 50);
    return s;
}

int main(int argc, char *argv[])
{
    srand((unsigned int) time(NULL));

    Suite *announce = announce_suite();
    SRunner *test_runner = srunner_create(announce);

    int number_failed = 0;
    srunner_run_all(test_runner, CK_VERBOSE);
    number_failed = srunner_ntests_failed(test_runner);

    srunner_free(test_runner);

    return number_failed;
    return;
}
