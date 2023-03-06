#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

int main(int argc, char **argv) {
    // Initialize the SNMP library
    init_snmp("snmpapp");

    // Open an SNMP session with the router
    netsnmp_session session, *ss;
    init_snmp("snmpapp");
    snmp_sess_init(&session);
    session.peername = strdup("192.168.1.1");
    session.version = SNMP_VERSION_2c;
    session.community = (u_char *)"public";
    session.community_len = strlen((const char *)session.community);
    ss = snmp_open(&session);

    if (!ss) {
        snmp_perror("snmp_open");
        exit(1);
    }

    // Retrieve the bandwidth and latency information using SNMP
    netsnmp_variable_list *vars;
    oid bandwidth_oid[] = {1, 3, 6, 1, 2, 1, 2, 2, 1, 5, 1}; // ifSpeed OID
    oid latency_oid[] = {1, 3, 6, 1, 2, 1, 4, 22, 1, 3, 1}; // icmpEchoMaxRTT OID
    const int bandwidth_oid_len = sizeof(bandwidth_oid)/sizeof(oid);
    const int latency_oid_len = sizeof(latency_oid)/sizeof(oid);

    // Retrieve bandwidth information
    snmpget(ss, bandwidth_oid, bandwidth_oid_len, &vars);
    if (vars) {
        printf("Bandwidth: %ld\n", *(vars->val.integer));
        snmp_free_var(vars);
    } else {
        printf("Error retrieving bandwidth information: %s\n", snmp_errstring(snmp_errno));
    }

    // Retrieve latency information
    snmpget(ss, latency_oid, latency_oid_len, &vars);
    if (vars) {
        printf("Latency: %ld\n", *(vars->val.integer));
        snmp_free_var(vars);
    } else {
        printf("Error retrieving latency information: %s\n", snmp_errstring(snmp_errno));
    }

    // Close the SNMP session
    snmp_close(ss);
    return 0;
}
